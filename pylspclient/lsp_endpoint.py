from __future__ import print_function
import threading
import collections
from pylspclient2 import lsp_structs
import zmq ############################################

class LspEndpoint(threading.Thread):
    def __init__(self, json_rpc_endpoint, method_callbacks={}, notify_callbacks={}):
        threading.Thread.__init__(self)
        self.json_rpc_endpoint = json_rpc_endpoint
        self.notify_callbacks = notify_callbacks
        self.method_callbacks = method_callbacks
        self.event_dict = {}
        self.response_dict = {}
        self.next_id = 0
        self.shutdown_flag = False
        #######################
        context = zmq.Context()
        socket = context.socket(zmq.PUB)
        socket.bind("tcp://*:5557")
        ##########################

    def handle_result(self, rpc_id, result, error):
        self.response_dict[rpc_id] = (result, error)
        cond = self.event_dict[rpc_id]
        cond.acquire()
        cond.notify()
        cond.release()


    def stop(self):
        self.shutdown_flag = True


    def run(self):
        while not self.shutdown_flag:
            try:
                jsonrpc_message = self.json_rpc_endpoint.recv_response()
                if jsonrpc_message is None:
                    print("server quit")
                    break
                print("received message:\n", jsonrpc_message)
                method = jsonrpc_message.get("method")
                result = jsonrpc_message.get("result")
                error = jsonrpc_message.get("error")
                rpc_id = jsonrpc_message.get("id")
                params = jsonrpc_message.get("params")

                if method:
                    if rpc_id:
                        # a call for method
                        if method not in self.method_callbacks:
                            raise lsp_structs.ResponseError(lsp_structs.ErrorCodes.MethodNotFound, "Client->Method not found: {method}".format(method=method))
                        result = self.method_callbacks[method](params)
                        self.send_response(rpc_id, result, None)
                    else:
                        # a call for notify
                        if method == "textDocument/publishDiagnostics":
                            if params.get('diagnostics'):
                                print(f"Diagnostic message: {params.get('diagnostics')[0].get('message')}\n")
                                start = params.get("diagnostics")[0].get("range").get("start")
                                print(f"Diagnostic range start: line: {start.get('line')}; char: {start.get('character')}\n")
                                socket.send("hello", f"Diagnostic range start: line: {start.get('line')}; char: {start.get('character')}")
                            else:
                                print("There were no errors\n")
                        if method not in self.notify_callbacks:
                            # Have nothing to do with this.
                            print("Client->Notify method not found: {method}.".format(method=method))
                        else:
                            self.notify_callbacks[method](params)
                else:
                    self.handle_result(rpc_id, result, error)
            except lsp_structs.ResponseError as e:
                self.send_response(rpc_id, None, e)


    def send_response(self, id, result, error):
        message_dict = {}
        message_dict["jsonrpc"] = "2.0"
        message_dict["id"] = id
        if result:
            message_dict["result"] = result
        if error:
            message_dict["error"] = error
        self.json_rpc_endpoint.send_request(message_dict)


    def send_message(self, method_name, params, id = None):
        message_dict = {}
        message_dict["jsonrpc"] = "2.0"
        if id is not None:
            message_dict["id"] = id
        message_dict["method"] = method_name
        message_dict["params"] = params
        self.json_rpc_endpoint.send_request(message_dict)


    def call_method(self, method_name, **kwargs):
        current_id = self.next_id
        self.next_id += 1
        cond = threading.Condition()
        self.event_dict[current_id] = cond

        cond.acquire()
        self.send_message(method_name, kwargs, current_id)
        if self.shutdown_flag:
            return None

        cond.wait()
        cond.release()

        self.event_dict.pop(current_id)
        result, error = self.response_dict.pop(current_id)
        if error:
            raise lsp_structs.ResponseError(error.get("code"), error.get("message"), error.get("data"))
        return result


    def send_notification(self, method_name, **kwargs):
        self.send_message(method_name, kwargs)
