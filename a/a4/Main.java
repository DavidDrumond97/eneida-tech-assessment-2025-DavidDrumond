public class Main {
    public static void main(String[] args) throws Exception {
        Logger logger = new Logger();

        logger.log("First message");
        logger.log("Second message");
        logger.log("Third message");

        // give some time for worker to print
        Thread.sleep(200);

        logger.stop();
    }
}
