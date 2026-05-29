# Forward Pass Guide (Hand-Holding)

This walks through **one image** through your network, using **your** variable names and shapes from `main.cpp`.

---

## What You're Building (30-Second Picture)

```
image_vector[img]  (784 bytes, 0–255)
        ↓  divide by 255
    input x[784]     (doubles, ~0–1)
        ↓  × weight1 + bias1, then ReLU
    layer1[128]      (hidden activations)
        ↓  × weight2 + bias2
    output_layer[10] (logits — raw scores per digit)
```

You already have the "boxes." Forward pass is **filling** `layer1` and `output_layer` from one row of `image_vector`.

---

## Your Shapes (Memorize These)

| Thing | Size | Meaning |
|--------|------|--------|
| `image_vector[img]` | 784 | One MNIST image, flattened |
| `weight1[j][i]` | 128 × 784 | Neuron `j`, input pixel `i` |
| `bias1[j]` | 128 | Bias for hidden neuron `j` |
| `layer1[j]` | 128 | Hidden activation after ReLU |
| `weight2[k][j]` | 10 × 128 | Output neuron `k`, hidden `j` |
| `bias2[k]` | 10 | Bias for digit `k` |
| `output_layer[k]` | 10 | Score for digit `k` |

**Rule:** row index = neuron in **this** layer, column index = value from **previous** layer.

---

## Step 0 — Tiny Prep (Do Once)

**0a.** Add at top of file (if missing):

```cpp
#include <algorithm>  // for std::max in relu
```

**0b.** Change activation vectors to start at zero (they get overwritten):

```cpp
std::vector<double> layer1(128, 0.0);
std::vector<double> output_layer(10, 0.0);
```

Weights/biases can stay as-is for now; you're only testing that math runs.

**0c.** Pick constants so you don't magic-number everywhere:

```cpp
const int INPUT_SIZE = 784;
const int HIDDEN_SIZE = 128;
const int OUTPUT_SIZE = 10;
```

(Or use `pixels_per_image` for 784 if you prefer.)

**0d.** Pick **one** training example to debug:

```cpp
const uint32_t img = 0;  // first image
```

You'll compare later with `label_vector[img]`.

---

## Step 1 — Build the Input Vector `x`

**Goal:** Turn bytes into doubles the network can multiply.

**Why divide by 255?** Pixels are 0–255; weights are ~0.1. Without scaling, products blow up. Training usually uses **0.0 to 1.0**.

**What to write:** A `std::vector<double> x` of length 784.

```cpp
std::vector<double> x(pixels_per_image);
for (uint32_t i = 0; i < pixels_per_image; i++) {
    x[i] = static_cast<double>(image_vector[img][i]) / 255.0;
}
```

**Checkpoint 1 — print a few pixels**

```cpp
std::cout << "label: " << static_cast<int>(label_vector[img]) << "\n";
std::cout << "x[0]=" << x[0] << " x[100]=" << x[100] << " x[783]=" << x[783] << "\n";
```

- Label should be 0–9.
- `x[i]` should be between 0 and 1.
- Mostly background → many values near 0; stroke pixels higher.

If that looks wrong, fix parsing before continuing.

---

## Step 2 — Hidden Layer (Pre-Activation → ReLU)

**Math for one hidden neuron `j`:**

\[
z_j = b1_j + \sum_{i=0}^{783} W1_{j,i}\, x_i
\]

\[
layer1_j = \mathrm{ReLU}(z_j) = \max(0, z_j)
\]

**In code — two loops:**

```cpp
for (int j = 0; j < 128; j++) {
    double z = bias1[j];

    for (int i = 0; i < 784; i++) {
        z += weight1[j][i] * x[i];
    }

    layer1[j] = relu(z);
}
```

**Order matters:**

1. Start with `z = bias1[j]` only.
2. Add each `weight1[j][i] * x[i]`.
3. **Then** `relu` — not before the sum.

**Checkpoint 2 — print hidden stats**

```cpp
double sum = 0;
for (int j = 0; j < 128; j++) sum += layer1[j];
std::cout << "layer1 sum=" << sum << " layer1[0]=" << layer1[0] << "\n";
```

- You should get **128 numbers**, many ≥ 0 (ReLU kills negatives).
- With all weights 0.1 and all `x[i]` ~ small, `z` is large → many ReLUs "on." That's OK for a **first plumbing test**; random small weights come later.

**Common mistake:** Using `weight1[i][j]` — you switched to **128 rows, 784 cols**, so it's always `weight1[j][i]`.

---

## Step 3 — Output Layer (Logits, No ReLU Yet)

**Math for one output neuron `k`:**

\[
output_k = b2_k + \sum_{j=0}^{127} W2_{k,j}\, layer1_j
\]

**No ReLU on the last layer** for now (softmax comes in the loss step).

```cpp
for (int k = 0; k < 10; k++) {
    double z = bias2[k];

    for (int j = 0; j < 128; j++) {
        z += weight2[k][j] * layer1[j];
    }

    output_layer[k] = z;
}
```

**Checkpoint 3 — print all 10 logits**

```cpp
std::cout << "logits: ";
for (int k = 0; k < 10; k++)
    std::cout << output_layer[k] << " ";
std::cout << "\n";
```

- Exactly **10** values.
- They can be positive or negative; that's normal before softmax.

**Optional — "guess" digit (argmax)**

```cpp
int guess = 0;
for (int k = 1; k < 10; k++) {
    if (output_layer[k] > output_layer[guess])
        guess = k;
}
std::cout << "guess=" << guess << " true=" << static_cast<int>(label_vector[img]) << "\n";
```

With constant weights, guess is meaningless — you're checking the **pipeline runs**, not accuracy.

---

## Step 4 — Put It in One Place in `main`

After your weight/bias setup (around line 79), add a block like:

```
// --- forward pass (one image) ---
const uint32_t img = 0;

// Step 1: x
// Step 2: layer1
// Step 3: output_layer
// Checkpoints: prints
```

Build → run → read prints. **Done with forward pass phase** when all three checkpoints look sane.

---

## Visual: One Neuron at a Time

**Hidden neuron `j=0`:**

```
x[0]──×W[0][0]──┐
x[1]──×W[0][1]──┤
  ...           ├──(+bias1[0])──→ ReLU ──→ layer1[0]
x[783]×W[0][783]┘
```

**Output neuron `k=3` (digit "3"):**

```
layer1[0]──×W2[3][0]──┐
  ...                  ├──(+bias2[3])──→ output_layer[3]
layer1[127]×W2[3][127]┘
```

---

## Checklist (Tick as You Go)

- [ ] `x` built from `image_vector[img]` with `/ 255.0`
- [ ] Hidden loop: `j` outer, `i` inner, `weight1[j][i]`
- [ ] `relu` applied **once per** `layer1[j]`
- [ ] Output loop: `k` outer, `j` inner, `weight2[k][j]`
- [ ] No ReLU on `output_layer`
- [ ] Printed label + 10 logits
- [ ] Program compiles and runs without crash

---

## Common Bugs

| Symptom | Likely cause |
|--------|----------------|
| Garbage / huge numbers | Forgot `/ 255.0` or wrong index `weight1[i][j]` |
| All `layer1` identical | Symmetric init + same ops (expected with constant 0.1 weights) |
| Segfault | Loop bound wrong (e.g. `i < 128` in inner input loop) |
| `std::max` error | Missing `#include <algorithm>` |
| Wrong image | Wrong `img` index |

---

## After Forward Pass Works

1. Wrap Steps 1–3 in a function, e.g. `forward(img, ...)`.
2. Try `img = 1`, `img = 42` — logits should change.
3. Later: softmax + loss + backprop (not part of this phase).

---

## Minimal "Full Forward" Pseudocode (Copy Structure)

```cpp
const uint32_t img = 0;

std::vector<double> x(pixels_per_image);
for (uint32_t i = 0; i < pixels_per_image; i++)
    x[i] = static_cast<double>(image_vector[img][i]) / 255.0;

for (int j = 0; j < 128; j++) {
    double z = bias1[j];
    for (int i = 0; i < 784; i++)
        z += weight1[j][i] * x[i];
    layer1[j] = relu(z);
}

for (int k = 0; k < 10; k++) {
    double z = bias2[k];
    for (int j = 0; j < 128; j++)
        z += weight2[k][j] * layer1[j];
    output_layer[k] = z;
}

// prints + argmax optional
```

Type this into `main` yourself line by line; run after each checkpoint so you see what broke if something fails.
