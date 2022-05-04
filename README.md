### A multithreaded alarm

This repository implements a multithreaded alarm with a client and server thread. The client thread will not block while
the server waits for alarms. A binary queue is used to store alarms.