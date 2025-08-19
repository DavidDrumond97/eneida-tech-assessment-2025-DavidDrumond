import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

public class Logger {
    private final BlockingQueue<String> queue = new LinkedBlockingQueue<>();
    private final Thread worker;
    private volatile boolean running = true;

    public Logger() {
        worker = new Thread(() -> {
            try {
                while (running || !queue.isEmpty()) {
                    String msg = queue.take();
                    System.out.println(msg); // just print to console
                }
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
        });
        worker.start();
    }

    public void log(String message) {
        if (running) {
            queue.offer(message);
        }
    }

    public void stop() throws InterruptedException {
        running = false;
        worker.join();
    }
}

