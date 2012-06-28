K.load("task.k");
K.load("setting.k");

void main() {
	String logpoolip = getLogPoolIP();
	Task test = new Task(1, "test", 1, "eval", logpoolip, "p(1);");
	test.sendTo("localhost", "/dse/command/");
	return;
}

main();
