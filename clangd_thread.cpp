#include "pstream.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <time.h>
#include <fmt/core.h>
#include <string>
#include <nlohmann/json.hpp>

using namespace redi;
using json = nlohmann::json;

void do_something(std::string);
void readsome(pstream &, int);
char buf[1024]; //char buf[1024]{};
std::streamsize n;
std::string s;
json js;
std::string header;

int main() {
const pstreams::pmode mode = pstreams::pstdout|pstreams::pstdin;
pstream clangd("clangd --log=error", mode); //verbose or error or info

  std::thread t0([&]() {  
int pid = ::getpid();
s = R"({"jsonrpc": "2.0", "id": 0, "method": "initialize", "params": {"processId": 0, "rootPath": null, "rootUri": "file:///home/slzatz/pylspclient/", "initializationOptions": null, "capabilities": {"offsetEncoding": ["utf-8"], "textDocument": {"codeAction": {"dynamicRegistration": true}, "codeLens": {"dynamicRegistration": true}, "colorProvider": {"dynamicRegistration": true}, "completion": {"completionItem": {"commitCharactersSupport": true, "documentationFormat": ["markdown", "plaintext"], "snippetSupport": true}, "completionItemKind": {"valueSet": [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25]}, "contextSupport": true, "dynamicRegistration": true}, "definition": {"dynamicRegistration": true}, "documentHighlight": {"dynamicRegistration": true}, "documentLink": {"dynamicRegistration": true}, "documentSymbol": {"dynamicRegistration": true, "symbolKind": {"valueSet": [1, 2, 3, 4, 5, 6, 7, 8, 9,10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26]}}, "formatting": {"dynamicRegistration": true}, "hover": {"contentFormat": ["markdown", "plaintext"], "dynamicRegistration": true}, "implementation": {"dynamicRegistration": true}, "onTypeFormatting": {"dynamicRegistration": true}, "publishDiagnostics": {"relatedInformation": true}, "rangeFormatting": {"dynamicRegistration": true}, "references": {"dynamicRegistration": true}, "rename": {"dynamicRegistration": true}, "signatureHelp": {"dynamicRegistration": true, "signatureInformation": {"documentationFormat": ["markdown", "plaintext"]}}, "synchronization": {"didSave": true, "dynamicRegistration": true, "willSave": true, "willSaveWaitUntil": true}, "typeDefinition": {"dynamicRegistration": true}}, "workspace": {"applyEdit": true, "configuration": true, "didChangeConfiguration": {"dynamicRegistration": true}, "didChangeWatchedFiles": {"dynamicRegistration": true}, "executeCommand": {"dynamicRegistration": true}, "symbol": {"dynamicRegistration": true, "symbolKind": {"valueSet": [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26]}}, "workspaceEdit": {"documentChanges": true}, "workspaceFolders": true}}, "trace": "off", "workspaceFolders": [{"name": "python-lsp", "uri": "file:///home/slzatz/pylspclient/"}]}})";

js = json::parse(s);
js["params"]["processId"] = pid + 1;
s = js.dump();

header = fmt::format("Content-Length: {}\r\n\r\n", s.size());
s = header + s;
fmt::print("\nsending initialization message to clangd:\n{}\n", s);
clangd.write(s.c_str(), s.size()).flush();


//initialization from client produces a capabilities response
//from the server which is read below
//the server response begins Content-Length: 828 + json response
/*
std::string str;
std::cout << '\n';
std::getline(clangd, str);
std::cout << "getline: " << str << '\n';
std::getline(clangd, str);
std::cout << "getline: " << str << '\n';
*/
readsome(clangd, 1); //this could block

//std::this_thread::sleep_for((std::chrono::seconds(2)));
//Client sends initialized response
s = R"({"jsonrpc": "2.0", "method": "initialized", "params": {}})";
header = fmt::format("Content-Length: {}\r\n\r\n", s.size());
s = header + s;
fmt::print("\nsending initialized message to clangd:\n{}\n", s);
clangd.write(s.c_str(), s.size()).flush();

//std::this_thread::sleep_for((std::chrono::seconds(2)));

//readsome(clangd, 2); //doesn't read anything so could be eliminated

//std::this_thread::sleep_for((std::chrono::seconds(1)));

//client sends didOpen notification
s = R"({"jsonrpc": "2.0", "method": "textDocument/didOpen", "params": {"textDocument": {"uri": "file:///home/slzatz/pylspclient/test.cpp", "languageId": "cpp", "version": 1, "text": ""}}})";
js = json::parse(s);
js["params"]["textDocument"]["text"] = " "; //text ? if it escapes automatically
s = js.dump();

s = R"({"jsonrpc": "2.0", "method": "textDocument/didOpen", "params": {"textDocument": {"uri": "file:///home/slzatz/pylspclient/test.cpp", "languageId": "cpp", "version": 1, "text": "#include \"pstream.h\"\n#include <iostream>\n#include <thread>\n#include <chrono>\n#include <time.h>\n#include <fmt/core.h>\n#include <string>\n\nusing namespace redi;\n\nint main() {\n\n\nconst pstreams::pmode mode = pstreams::pstdout|pstreams::pstdin;\npstream child(\"python python_server.py\", mode);\nchar buf[1024];\nstd::streamsize n;\nint i = 0;\ntime_t t0 = time(NULL);\nwhile (1) {\n\n  if (i%5 == 0) {\n  auto t = time(NULL) - t0;\n  std::cout << \"writing to child: \" << t << '\\n';\n  std::string s = fmt::format(\"hello: {} - {}\\n\", i,t); \n  // flush below important to not buffering\n  // and therefore delaying the child's read\n  child.write(s.c_str(), s.size()).flush();\n }\n\n  std::cout << \"this is what server got\" << '\\n';\n  while ((n = child.out().readsome(buf, sizeof(buf))) > 0)\n     std::cout.write(buf, n).flush();\n\n  i += 1;\n\n  std::this_thread::sleep_for((std::chrono::seconds(5)));\n}\n\nreturn 0;\n}"}}})";
header = fmt::format("Content-Length: {}\r\n\r\n", s.size());
s = header + s;
fmt::print("\nsending didOpen message to clangd:\n{}\n", s);
clangd.write(s.c_str(), s.size()).flush();

//std::this_thread::sleep_for((std::chrono::seconds(2)));
//client receives a diagnostics response from its didOpen notification
/*
std::cout << '\n';
std::getline(clangd, str);
std::cout << "getline: " << str << '\n';
std::getline(clangd, str);
std::cout << "getline: " << str << '\n';
*/
//clangd >> str;
//std::cout << "clangd >> str: " << str << '\n';
readsome(clangd, 3);//sometimes too soon and sometimes not this could block
//reads initial diagnostics
});

//need this delay
std::this_thread::sleep_for((std::chrono::seconds(20)));
//t0.join();
//char x;
////std::cin >> x;

// may not need the second thread
std::atomic<bool> code_changed = false;
std::atomic<bool> active = true;
std::string code;
std::cout << "Launching second thread" << '\n';
std::thread t1([&]() {  
int j = 1;

std::string dC_str = R"({"jsonrpc": "2.0", "method": "textDocument/didChange", "params": {"textDocument": {"uri": "file:///home/slzatz/pylspclient/test.cpp", "version": 2}, "contentChanges": [{"text": ""}]}})";

json dC_js = json::parse(dC_str);
while (active) {
  //if (file_change) 
  //1) get changes sendDidChange
  //2. construct text string out of rows
  //3. construct didChange json
  //4. send the didChange notification
  //5. get the response from the server
  //6. call decorate_error
  //else
  //1. sleep for a little bit
  if (code_changed) {
    dC_js["params"]["contentChanges"][0]["text"] = code; //text ? if it escapes automatically
    dC_js["params"]["textDocument"]["version"] = ++j; //text ? if it escapes automatically
    dC_str = dC_js.dump();
    header = fmt::format("Content-Length: {}\r\n\r\n", dC_str.size());
    dC_str = header + dC_str;
    fmt::print("\nsending didChange message to clangd:\n{}\n", dC_str);
    clangd.write(dC_str.c_str(), dC_str.size()).flush();

    readsome(clangd, j);
    code_changed = false;
  }
  std::this_thread::sleep_for((std::chrono::seconds(1)));
}

//below doesn't work because escapes \n with \\n
//std::string text = R"(#include <iostream>\nint main() {\n std::cout << "Hello World" << std::endl\n})";
std::string text = "#include <iostream>\nint main() {\n std::cout << \"Hello World\" << std::endl\n}";
//didChange
s = R"({"jsonrpc": "2.0", "method": "textDocument/didChange", "params": {"textDocument": {"uri": "file:///home/slzatz/pylspclient/test.cpp", "version": 2}, "contentChanges": [{"text": ""}]}})";

/*
js = json::parse(s);
js["params"]["contentChanges"][0]["text"] = text; //text ? if it escapes automatically
js["params"]["textDocument"]["version"] = 2; //text ? if it escapes automatically
s = js.dump();
header = fmt::format("Content-Length: {}\r\n\r\n", s.size());
s = header + s;
fmt::print("\nsending didChange message to clangd:\n{}\n", s);
clangd.write(s.c_str(), s.size()).flush();
*/
//seems like you could block since every didChange should
//get a textDocument/publishDiagnostics response

});

std::this_thread::sleep_for((std::chrono::seconds(5)));
code_changed = true;
code = "#include <iostream>\nint main() {\n std::cout << \"Hello World\" << std::endl\n}";
std::this_thread::sleep_for((std::chrono::seconds(5)));
code_changed = true;
code = "#include <iostream>\nint main() {\n std::cout << \"Hello World\" << std::endl;\n}";
std::this_thread::sleep_for((std::chrono::seconds(5)));
code_changed = true;
code = "#include <iostream>\nint main() {\n std::count << \"Hello World\" << std::endl;\n}";
std::this_thread::sleep_for((std::chrono::seconds(5)));
code_changed = true;
code = "#include <iostream>\nint main() {\n std::cout << \"Hello World\" << std::endl;\n}";
std::this_thread::sleep_for((std::chrono::seconds(5)));
active = false;
std::this_thread::sleep_for((std::chrono::seconds(1)));

std::cout << "attempting to join thread 0" << '\n';
t0.join();
std::cout << "attempting to join thread 1" << '\n';
t1.join();
return 0;
}
//looks like the buffer holds on to last bits until a new read takes place
void readsome(pstream &p, int i) {
  fmt::print("\nbeginning read {}\n", i);
  std::string str{};
  std::string header{};
  std::cout << '\n';
  std::getline(p, header);
  str = header;
  std::cout << "getline: " << str << '\n';
  std::getline(p, header);
  str += header;
  std::cout << "getline: " << str << '\n';
  //char buf[1024];
  //std::streamsize n;
  while ((n = p.out().readsome(buf, sizeof(buf))) > 0) {
    // n will always be zero eventually
     std::cout.write(buf, n).flush(); //when n = 0 nothing shown even though buffer has stuff
     //s += std::string{buf, (size_t)n};
     str += std::string{buf, static_cast<size_t>(n)};
  }
  // should always be non-empty
  if (str.size()) do_something(str);
  buf[0] = '\0';
  fmt::print("\nend read {}\n", i);
}

void do_something(std::string s) {
  //std::string_view sv{buf};
  std::cout << "\n\ndo_something: " << s.substr(0, 20);
}
