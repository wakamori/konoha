K.import("konoha");
K.import("apache");

int System.handler(Request req) {
     req.setContentType("text/html")
     req.setContentEncoding("utf-8")
     req.logError(APLOG_WARNING, 0, "log error's test")
     req.puts("<p>hello world!</p>\n")
     req.puts("<p>"+req.getMethod()+"</p>\n")
     req.puts("<p>"+req.getArgs()+"</p>\n")
     req.puts("<p>"+req.getUri()+"</p>\n")
     req.puts("<p>PATH_INFO:" + req.getPathInfo()+"</p>\n")
     req.puts("<p>"+req.getHandler()+"</p>\n")
     req.puts("<p>"+"logのテスト</p>\n")
     req.puts("<p>"+"request & response</p>\n")
     AprTable reqHeaders = req.getHeadersIn();
     AprTable resHeaderTable = req.getHeadersOut();
     resHeaderTable.add("test", "val1");
     resHeaderTable.add("test", "val2");
     resHeaderTable.getElts();
     resHeaderTable.set("test", "val3");
     resHeaderTable.add("test", "val4");
     resHeaderTable.add("test", "値5");
     AprTableEntry[] test = resHeaderTable.getElts();
     req.puts("<p>size = " + test.getSize() + "</p>\n");
     int i = 0;
     while (i<test.getSize()) {
          req.puts("<p>"+test[i].getKey() + "=>" + test[i].getVal() + "</p>\n");
          i = i+1;
     }
     return APACHE_OK;
}
 