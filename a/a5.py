import math
import cmath

def rms(sig):
    return math.sqrt(sum(x*x for x in sig) / len(sig))

def power_factor(v, i):
    p = sum(v[k]*i[k] for k in range(len(v))) / len(v)
    s = rms(v) * rms(i)
    return p / s if s != 0 else 0.0

def thd(sig, fs, f1):
    N = len(sig)
    # DFT at fundamental
    X1 = sum(sig[k]*cmath.exp(-2j*math.pi*1*f1*k/fs) for k in range(N))
    A1 = abs(X1) / N
    # Sum of harmonics (2..10 for simplicity)
    harm = 0.0
    for n in range(2,11):
        Xn = sum(sig[k]*cmath.exp(-2j*math.pi*n*f1*k/fs) for k in range(N))
        An = abs(Xn) / N
        harm += An*An
    return math.sqrt(harm) / A1 if A1 != 0 else 0.0

# demo with made-up sine waves
if __name__ == "__main__":
    fs = 8000
    f1 = 50
    N = int(fs/f1*20)     # 20 cycles
    t = [k/fs for k in range(N)]
    v = [math.sin(2*math.pi*f1*tk) for tk in t]
    i = [math.sin(2*math.pi*f1*tk - 0.3) for tk in t]  # lagging current

    pf = power_factor(v, i)
    thd_v = thd(v, fs, f1)

    print("Power Factor:", round(pf, 3))
    print("THD (voltage):", round(thd_v, 3))
