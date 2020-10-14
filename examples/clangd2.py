import pylspclient2 as pylspclient
#import pylspclient
import subprocess
import threading
import argparse

# seems only here to handle stderr
class ReadPipe(threading.Thread):
    def __init__(self, pipe):
        threading.Thread.__init__(self)
        self.pipe = pipe

    def run(self):
        line = self.pipe.readline().decode('utf-8')
        while line:
            print(line)
            line = self.pipe.readline().decode('utf-8')

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='pylspclient example with clangd')
    parser.add_argument('clangd_path', type=str, default="/usr/bin/clangd", 
                    help='the clangd path', nargs="?")
    args = parser.parse_args()

    print("### clangd launched through subprocess.Popen")
    p = subprocess.Popen([args.clangd_path], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    # ReadPipe class only used for stderr
    read_pipe = ReadPipe(p.stderr)
    read_pipe.start()



    json_rpc_endpoint = pylspclient.JsonRpcEndpoint(p.stdin, p.stdout)
    # To work with socket: sock_fd = sock.makefile()
    lsp_endpoint = pylspclient.LspEndpoint(json_rpc_endpoint)
    lsp_client = pylspclient.LspClient(lsp_endpoint)


    capabilities = {'offsetEncoding': ['utf-8'],
    'textDocument': {'codeAction': {'dynamicRegistration': True},
    'codeLens': {'dynamicRegistration': True},
    'colorProvider': {'dynamicRegistration': True},
    'completion': {'completionItem': {'commitCharactersSupport': True,
        'documentationFormat': ['markdown', 'plaintext'],
        'snippetSupport': True},
    'completionItemKind': {'valueSet': [1,
        2,
        3,
        4,
        5,
        6,
        7,
        8,
        9,
        10,
        11,
        12,
        13,
        14,
        15,
        16,
        17,
        18,
        19,
        20,
        21,
        22,
        23,
        24,
        25]},
    'contextSupport': True,
    'dynamicRegistration': True},
    'definition': {'dynamicRegistration': True},
    'documentHighlight': {'dynamicRegistration': True},
    'documentLink': {'dynamicRegistration': True},
    'documentSymbol': {'dynamicRegistration': True,
    'symbolKind': {'valueSet': [1,
        2,
        3,
        4,
        5,
        6,
        7,
        8,
        9,
        10,
        11,
        12,
        13,
        14,
        15,
        16,
        17,
        18,
        19,
        20,
        21,
        22,
        23,
        24,
        25,
        26]}},
    'formatting': {'dynamicRegistration': True},
    'hover': {'contentFormat': ['markdown', 'plaintext'],
    'dynamicRegistration': True},
    'implementation': {'dynamicRegistration': True},
    'onTypeFormatting': {'dynamicRegistration': True},
    'publishDiagnostics': {'relatedInformation': True},
    'rangeFormatting': {'dynamicRegistration': True},
    'references': {'dynamicRegistration': True},
    'rename': {'dynamicRegistration': True},
    'signatureHelp': {'dynamicRegistration': True,
    'signatureInformation': {'documentationFormat': ['markdown', 'plaintext']}},
    'synchronization': {'didSave': True,
    'dynamicRegistration': True,
    'willSave': True,
    'willSaveWaitUntil': True},
    'typeDefinition': {'dynamicRegistration': True}},
    'workspace': {'applyEdit': True,
    'configuration': True,
    'didChangeConfiguration': {'dynamicRegistration': True},
    'didChangeWatchedFiles': {'dynamicRegistration': True},
    'executeCommand': {'dynamicRegistration': True},
    'symbol': {'dynamicRegistration': True,
    'symbolKind': {'valueSet': [1,
        2,
        3,
        4,
        5,
        6,
        7,
        8,
        9,
        10,
        11,
        12,
        13,
        14,
        15,
        16,
        17,
        18,
        19,
        20,
        21,
        22,
        23,
        24,
        25,
        26]}},'workspaceEdit': {'documentChanges': True},
    'workspaceFolders': True}}

    root_uri = 'file:///home/slzatz/pylspclient/examples/'

    workspace_folders = [{'name': 'python-lsp', 'uri': root_uri}]

    print("\n### client sends initialize:\n")
    print(lsp_client.initialize(p.pid, None, root_uri, None, capabilities, "off", workspace_folders)) #no apparent change when off->verbose
    print("\n### client sends initialized:\n")
    print(lsp_client.initialized())

    file_path = "/home/slzatz/pylspclient/examples/test.cpp"
    uri = "file://" + file_path
    text = open(file_path, "r").read()
    text2 = "#include <iostream>\n#include <string>\n#include <vector>\n\nint main() {\nstd::vector<int> v{1,2,3,4};\nstd::st"\
            "ring s{\"Hello World\"};\n\nstd::cout << s << std::endl;\nstd::cout << v.at(2) << std::endl;\nreturn 0;\n\n}\n"
    languageId = pylspclient.lsp_structs.LANGUAGE_IDENTIFIER.CPP
    version = 1

    print(dir(lsp_client))
    #def didOpen(self, textDocument):
    print("\n### client sends didOpen:\n")
    lsp_client.didOpen(pylspclient.lsp_structs.TextDocumentItem(uri, languageId, version, text))


    p0 = pylspclient.lsp_structs.Position(6, 27)
    p1 = pylspclient.lsp_structs.Position(6,28)
    range_ = pylspclient.lsp_structs.Range(p0, p1)

    # the below works
    #lsp_client.didChange(pylspclient.lsp_structs.VersionedTextDocumentIdentifier(uri, 2),
    #                         pylspclient.lsp_structs.TextDocumentContentChangeEvent(range_, 1, "};"))

    lsp_client.didChange(pylspclient.lsp_structs.VersionedTextDocumentIdentifier(uri, 3), {"text": text2})

    try:
        print("\n### client sends documentSymbol:\n")
        symbols = lsp_client.documentSymbol(pylspclient.lsp_structs.TextDocumentIdentifier(uri))
        for symbol in symbols:
            print(symbol.name)
    except pylspclient.lsp_structs.ResponseError:
        # documentSymbol is supported from version 8.
        print("Failed to document symbols")

    #lsp_client.definition(pylspclient.lsp_structs.TextDocumentIdentifier(uri), pylspclient.lsp_structs.Position(14, 4))
    #lsp_client.signatureHelp(pylspclient.lsp_structs.TextDocumentIdentifier(uri), pylspclient.lsp_structs.Position(14, 4))
    #lsp_client.definition(pylspclient.lsp_structs.TextDocumentIdentifier(uri), pylspclient.lsp_structs.Position(14, 4))
    #lsp_client.completion(pylspclient.lsp_structs.TextDocumentIdentifier(uri), pylspclient.lsp_structs.Position(14, 4), pylspclient.lsp_structs.CompletionContext(pylspclient.lsp_structs.CompletionTriggerKind.Invoked))
    lsp_client.shutdown()
    lsp_client.exit()
