K.import("konoha");
K.import("apache");

int System.handler(Request r) {
    r.puts("<strong>hello world!</strong>")
    return APACHE_OK;
}
