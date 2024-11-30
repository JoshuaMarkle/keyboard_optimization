# GPU Implementation

Although this has not yet been imtegrated within the main application, the performance difference is incredible. Because the main bottleneck of this simulation is the keyboard value function, each keyboard in a generation can be run in parallel on the GPU and produce incredible performance gains.

On my machine without optimization flags:

```json
{
    "population_size": 1000,
    "generations": 1000
}
```

```bash
 g++ optimize.cpp -o normal_imlementation
 time ./normal_implementation
./normal_implementation  10.01s user 0.00s system 99% cpu 10.027 total
```

```bash
 nvcc main.cu -o cuda_implementation
 time ./cuda_implementation
./cuda_implementation  0.13s user 0.48s system 87% cpu 0.692 total
```

So, the CUDA GPU is $(\frac{0.692}{10.027}-1)\times100$ = 1349% faster!

This test was performed with these specs:

```
 nvidia-smi
+-----------------------------------------------------------------------------------------+
| NVIDIA-SMI 565.57.01              Driver Version: 565.57.01      CUDA Version: 12.7     |
|-----------------------------------------+------------------------+----------------------+
| GPU  Name                 Persistence-M | Bus-Id          Disp.A | Volatile Uncorr. ECC |
| Fan  Temp   Perf          Pwr:Usage/Cap |           Memory-Usage | GPU-Util  Compute M. |
|                                         |                        |               MIG M. |
|=========================================+========================+======================|
|   0  NVIDIA GeForce RTX 3050 ...    Off |   00000000:01:00.0 Off |                  N/A |
| N/A   50C    P8              8W /   15W |      12MiB /   6144MiB |      1%      Default |
|                                         |                        |                  N/A |
+-----------------------------------------+------------------------+----------------------+
```
