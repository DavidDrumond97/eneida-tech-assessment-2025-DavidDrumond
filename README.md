# Section B – Embedded Systems Skills

## Section A – Programming Languages ##

**A-1 – C: Fixed-Point RMS**

Compile and run the code
```bash
gcc a1.c -o a1 -lm
./a1
```
**A-2 – C: Circular Buffer**
Compile and run the code
```bash
gcc a2.c -o a2
./a2
```
If You try to push and the count is the sixe os the buffer does not write anithing ti the buffer.
If you try to pop and the count is zero, meaning its empty, it does not pop anyting.

**A-3 – C++: A Message Receiver**
Compile and run the code
```bash
g++ -std=c++17 a3.cpp -o a3
./a3
```

**A-4 – Java: Threaded Logger**
Compile and run the code
```bash
javac Logger.java Main.java
java Main
```

**A-5 – Power Factor and THD**
Run the code
```bash
python a5.py
```

**A-Bonus 2 – C: Where is the Bug?**

## a) Is the code below compilable using standard `gcc`?

```c
#include <stdint.h>
#include <stdio.h>

int main(void) {
    uint8_t i;
    for (i = 250; i <= 1050; i++) {
        printf("i=%d\n", i);
    }
    return 0;
}
```

Is complient , but the variable "i" overflows.

## b) Identify the bug(s) in the code snippet and explain them.

The variable "i" overflows. The loop never ends.

## B-1 – Interfacing with an External SPI Peripheral
1. **SPI initialization and configuration**  
   I would configure the SPI for 8-bit data, MSB first, and start at 500 kHz clock so communication is stable.  
   Chip Select is a GPIO: I set it low before sending and high after the transfer.  
   This way I can read or write multi-byte registers without problems.

2. **Driver architecture: blocking vs. non-blocking (interrupt or DMA-based) communication**  
   Would chose either an interrupt based approach or DMA so that the implementation gets non blocking.

3. **Abstraction layers (e.g., hardware abstraction vs. application-level API)**  
   At the hardware abstraction layer, I would handle the SPI details: init, clock, chip select, and raw read/write functions.  
   On top of that, the device driver layer knows the register map and exposes simple functions like `readRegister16()` or `writeRegister()`.  
   Finally, the application layer just calls clean APIs like `getTemperature()` without worrying about SPI.

4. **Handling multi-byte register reads/writes**  
   For multi-byte registers I keep chip select low for the whole transfer.  
   I send the starting register address, then clock out or in the required number of bytes.

5. **Power-on initialization and error checking mechanisms**
   On power-up I reset the device (if a reset pin is available), wait the required delay, then read
   the ID register to confirm the chip is responding. After that I configure
   the basic settings (SPI mode, ranges).

    ```text
    function readReg16(address):
        CS_LOW()
        spi_send(address | READ_BIT)
        spi_value = spi_receive()
        CS_HIGH()
        return spi_value

---

## B-2 – PIC32MZ DMA
I’m not very familiar with the PIC32 family, but on STM32 the approach is straightforward and I expect it’s very similar on PIC32:

- Configure the ADC to trigger a conversion (timer or continuous mode).  
- Set up a DMA channel with the ADC data register as the source and a buffer in RAM as the destination.  
- Enable memory increment so each new sample goes into the next buffer location.  
- Configure the DMA transfer size and enable circular mode if we want continuous sampling.  
- Start ADC + DMA, and the hardware moves samples into memory automatically.  

**Example use case:** sampling a microphone or sensor signal continuously and processing it later, or moving incoming UART data into a buffer without CPU polling.  

**Advantages:** CPU isn’t tied up moving data, throughput is higher, power use is lower, and latency is reduced since transfers happen in hardware.  

---

## B-3 – Flash Wear Reduction
To reduce flash wear when logging events I would:

- Avoid rewriting the same sector, instead use wear leveling so writes are spread across the device.  
- Use a journal or circular log so new entries are appended rather than erasing and rewriting whole blocks.  
- Buffer data in RAM and write in larger chunks instead of very frequent single writes.  

---

## B-Bonus 1 – Current Sensors Expected Configurations
**a) Configurations:**  
- **Rogowski coil** → output is proportional to the derivative of current, so the system must integrate the signal (either in analog front-end or digitally). It usually also needs gain adjustment because the output voltage is small.  
- **Current Transformer (CT)** → output is directly proportional to current, so you mainly set scaling and maybe offset correction, no integration needed.  

**b) Hardware signal conditioning:**  
Yes, it changes. Rogowski needs an integrator stage (RC or op-amp circuit) and more amplification.  
Current transformer only needs scaling, either with voltage divider or amplification.  

---

## B-Bonus 2 – Optimizing Boot Time
**Ways to reduce boot time:**
- Bootloader: skip delays, load kernel quickly.  
- Kernel: build only needed drivers, remove extras.  
- Userspace: disable unused services, start app early.  
- Filesystem: use fast/simple FS, tune mount options.  
- Hardware: initialize peripherals later if possible.  

**How to measure:**
- `systemd-analyze` for total and per-service time.  
- Kernel dmesg timestamps.  
- Bootcharts or ftrace for detailed tracing.  

---

# Section C – Software Design Skills

## C-1 – OTA Update Strategy
I’ve never implemented OTA before, but I understand the main principles:

- Use two firmware slots (A/B) so the device can recover if the update fails.  
- Always verify signature and hash of the new image before running it.  
- Write to the inactive slot, then only confirm after a successful boot.  
- Deliver updates over TLS and prevent rollback to older, insecure versions.  

---

## C-2 – OTA Failure Modes
**Failure modes**
- Power loss during download or flashing  
- Corrupted or incomplete image  
- Wrong version or rollback attack  
- Boot failure after update  

**Resilience mechanisms**
- A/B slots so device can roll back to previous firmware  
- Verify hash + signature before activating image  
- Only switch to new firmware after a successful boot/health check  
- Use watchdog to recover if the new firmware hangs  
- Keep config/data in a separate partition so it isn’t lost  

---

## C-3 – Payload Efficiency over Mobile Networks
- **Communication protocol:** Use MQTT over TLS with a persistent session to avoid reconnect overhead.  

- **Data Transfer Object (DTO):**  
  - Electrical data (every 10 min): voltages, currents, PF, THD (~120 bytes)  
  - Health data (hourly): temperature, battery voltage, uptime, errors (~80 bytes)  

- **Estimated monthly data usage:**  
  - Electrical: 4320 × 120 B ≈ 0.49 MB  
  - Health: 720 × 80 B ≈ 0.06 MB  
  - **Total with protocol overhead:** ~1 MB/month  

- **Techniques to reduce usage:** Batch multiple samples per message, use short field names or binary encoding (CBOR), send only changes (delta reporting), and keep a persistent connection.  

---

## C-Bonus 1 – Network Outages
- Buffer data locally in non-volatile memory (e.g., flash, FRAM, SD card).  
- Use a circular log or queue to avoid overwriting until confirmed upload.  
- Tag each record with timestamp and sequence number for consistency.  
- When the network is back, retry transmission and support resume/ack.  
- Apply data compression or batching to send backlog efficiently.  

---

## C-Bonus 2 – Watchdog and Failsafe System
- **Hardware watchdog (WDT):** Enable a windowed WDT with a short timeout (e.g., 1–2 s). Only “kick” it from a supervisor task that checks system health.  
- **Supervisor/heartbeat:** Each critical task (comms, ADC, storage) updates a heartbeat flag. If any flag is stale, do not pet the WDT → automatic reset.  
- **Timeouts everywhere:** Add timeouts to SPI/UART/ADC/DMA ops; on timeout, reset the peripheral/bus, log, and retry with backoff.  
- **Fail-safe modes:** On repeated faults, enter degraded mode (reduced sampling / cached data) and keep minimal telemetry alive.  
- **Recovery on boot:** After WDT reset, read reset cause, persist an error counter, and optionally switch to a safe configuration/firmware (A/B) if crashes repeat.  
- **Prevents lock-ups because:** hung tasks stop heartbeats → WDT forces a clean reboot; stuck peripherals are re-initialized; the system always returns to a known-good state instead of staying frozen.  
