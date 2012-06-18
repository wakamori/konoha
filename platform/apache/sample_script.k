K.import("konoha");

class Wsgi {
	Wsgi(){
		System.p("init wsgi");
	}
	String application() {
		System.p("hello world");
		return "hello world";
	}
}

System.p("main");
Wsgi w = new Wsgi();
w.application();

