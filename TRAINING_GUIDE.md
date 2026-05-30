# Training Guide (Hand-Holding)

This walks through **one image** from logits to weight updates, using **your** variable names and shapes from `main.cpp`. You already have forward pass over all images — this adds loss, backprop, and SGD.

---

## What You're Building (30-Second Picture)

```
output_layer[10]  (logits — you have this)
        ↓  softmax
    probs[10]       (probabilities, sum = 1)
        ↓  compare to label_vector[im]
    loss            (one number — cross-entropy)
        ↓  backward
    deltas          (how wrong each neuron was)
        ↓  chain rule
    update weight1, weight2, bias1, bias2
```

Training = forward → loss → backward → update, **inside** your `for (im ...)` loop (once per image for now).

---

## New Vectors (Memorize These)

| Thing | Size | Meaning |
|--------|------|--------|
| `output_layer[k]` | 10 | Logits (before softmax) |
| `probs[k]` | 10 | Softmax output |
| `label` | 1 int | `label_vector[im]`, digit 0–9 |
| `delta2[k]` | 10 | Gradient at output layer |
| `delta1[j]` | 128 | Gradient at hidden layer |
| `z1[j]` | 128 | Hidden **pre-ReLU** value (save in forward pass) |

**Rule:** you need `z1` during forward pass to backprop through ReLU later.

---

## Step 0 — Tiny Prep (Do Once)

**0a.** Add learning rate near your constants:

```cpp
constexpr double LEARNING_RATE = 0.1;  // try 0.01 if training unstable
```

**0b.** During forward pass layer 1, **save pre-ReLU values**:

```cpp
std::vector<double> z1(HIDDEN_SIZE, 0.0);

// inside hidden loop, before relu:
z1[row] = weighted_sum;
layer1[row] = relu(weighted_sum);
```

**0c.** Add softmax + gradient buffers (per image, inside `im` loop or reused):

```cpp
std::vector<double> probs(OUTPUT_SIZE, 0.0); // this is softmax
std::vector<double> delta2(OUTPUT_SIZE, 0.0); // gradient 2
std::vector<double> delta1(HIDDEN_SIZE, 0.0); // gradient 1
```

**0d.** Get the true label for this image:

```cpp
int label = static_cast<int>(label_vector[im]);
```

---

## Step 1 — Softmax

**Goal:** Turn logits into probabilities that sum to 1.

**Math:**

\[
p_k = \frac{e^{z_k}}{\sum_{c=0}^{9} e^{z_c}}
\]

where \(z_k =\) `output_layer[k]`.

**Stability trick:** subtract max logit before `exp` (prevents overflow).

```cpp
double max_logit = output_layer[0];
for (int k = 1; k < OUTPUT_SIZE; k++) {
    if (output_layer[k] > max_logit)
        max_logit = output_layer[k];
}

double sum_exp = 0.0;
for (int k = 0; k < OUTPUT_SIZE; k++) {
    probs[k] = std::exp(output_layer[k] - max_logit);
    sum_exp += probs[k];
}
for (int k = 0; k < OUTPUT_SIZE; k++) {
    probs[k] /= sum_exp;
}
```

**Checkpoint 1 — print probs**

```cpp
double prob_sum = 0.0;
for (int k = 0; k < OUTPUT_SIZE; k++) prob_sum += probs[k];
std::cout << "prob_sum=" << prob_sum << " p[label]=" << probs[label] << "\n";
```

- `prob_sum` should be **1.0** (or very close).
- `probs[label]` should be in `(0, 1)`.

---

## Step 2 — Cross-Entropy Loss

**Goal:** One number measuring how wrong the prediction is.

**Math (single correct class `label`):**

\[
L = -\log(p_{\text{label}})
\]

```cpp
double loss = -std::log(probs[label] + 1e-15);  // small epsilon avoids log(0)
```

**Checkpoint 2:** `loss` ~ **2.3** untrained; should drop over epochs.

---

## Step 3 — Output Layer Gradient (`delta2`)

**Goal:** How much each output logit should change. With softmax + cross-entropy, this is beautifully simple:

\[
\delta2_k = p_k - \mathbb{1}_{k = \text{label}}
\]

```cpp
for (int k = 0; k < OUTPUT_SIZE; k++) {
    delta2[k] = probs[k];
    if (k == label)
        delta2[k] -= 1.0;
}
```

**Checkpoint 3:** `delta2[label]` negative; all `delta2` sum to **0**.

---

## Step 4 — Hidden Layer Gradient (`delta1`)

**Goal:** Propagate error through `weight2`, then ReLU.

\[
\delta1_j = \left(\sum_{k=0}^{9} \delta2_k \cdot W2_{k,j}\right) \cdot \mathrm{ReLU}'(z1_j)
\]

ReLU derivative: 1 if `z1[j] > 0`, else 0.

```cpp
for (int j = 0; j < HIDDEN_SIZE; j++) {
  double grad = 0.0;
  for (int k = 0; k < OUTPUT_SIZE; k++) {
    grad += delta2[k] * weight2[k][j];
  }
  delta1[j] = (z1[j] > 0.0) ? grad : 0.0;
}
```

**Checkpoint 4:** `delta1` values small, not NaN.

---

## Step 5 — Update Weights and Biases (SGD)

**Goal:** Nudge parameters opposite to the gradient.

\[
W \leftarrow W - \eta \cdot \frac{\partial L}{\partial W}
\]

**Output layer:**

```cpp
for (int k = 0; k < OUTPUT_SIZE; k++) {
    bias2[k] -= LEARNING_RATE * delta2[k];
    for (int j = 0; j < HIDDEN_SIZE; j++) {
        weight2[k][j] -= LEARNING_RATE * delta2[k] * layer1[j];
    }
}
```

**Hidden layer:**

```cpp
for (int j = 0; j < HIDDEN_SIZE; j++) {
    bias1[j] -= LEARNING_RATE * delta1[j];
    for (int i = 0; i < INPUT_SIZE; i++) {
        weight1[j][i] -= LEARNING_RATE * delta1[j] * x[im][i];
    }
}
```

---

## Step 6 — Training Loop Structure

Inside your `for (int im ...)` loop, after forward: softmax → loss → `delta2` → `delta1` → update.

**Outer epoch loop:**

```cpp
constexpr int EPOCHS = 3;
for (int epoch = 0; epoch < EPOCHS; epoch++) {
    double epoch_loss = 0.0;
    for (int im = 0; im < NUM_IMAGE; im++) {
        // forward + backward + update
        epoch_loss += loss;
    }
    std::cout << "epoch " << epoch << " avg loss="
              << epoch_loss / NUM_IMAGE << "\n";
}
```

**Checkpoint 6 — average loss should decrease across epochs.**

---

## Visual: Error Flow Backward

```
label (digit 5)
    ↓
probs[10] ← softmax ← output_layer
    ↓
delta2[k] = probs[k] - (k==label ? 1 : 0)
    ↓
weight2, bias2  updated
    ↓
delta1[j]  (through ReLU gate on z1[j])
    ↓
weight1, bias1  updated
```

---

## Checklist (Tick as You Go)

- [ ] Save `z1[j]` before `relu` in forward pass
- [ ] Softmax: subtract max, divide by `sum_exp`
- [ ] Loss: `-log(probs[label])`
- [ ] `delta2[k] = probs[k] - (k == label)`
- [ ] `delta1[j]` uses `weight2[k][j]` and ReLU mask on `z1[j]`
- [ ] Update `weight2`, `bias2`, `weight1`, `bias1` with `LEARNING_RATE`
- [ ] All of the above inside `im` loop
- [ ] Optional: outer `EPOCHS` loop + print average loss

---

## Common Bugs

| Symptom | Likely cause |
|--------|----------------|
| `loss` is NaN | `log(0)` — add epsilon or fix softmax |
| Loss never drops | Learning rate too high/low; forgot to update weights |
| All `delta1` zero | ReLU dead — `z1` all ≤ 0; try smaller init or more epochs |
| Wrong gradients | Used `layer1` instead of `z1` for ReLU derivative |
| Weights explode | Learning rate too large — try `0.01` |
| Only last image matters | Updates outside `im` loop |
| `delta2` wrong | Forgot `delta2[label] -= 1` |

---

## After Training Works

1. Track **accuracy**: argmax on `probs` vs `label`, count correct / total.
2. Wrap forward + backward in functions to clean up `main`.
3. Later: mini-batches, test set (`t10k-*` files), save/load weights.

---

## Minimal "One Image Train Step" Pseudocode (Copy Structure)

```cpp
int label = static_cast<int>(label_vector[im]);
// forward (save z1[j] before relu) — same as your current loops
// softmax + loss — Steps 1–2
for (int k = 0; k < OUTPUT_SIZE; k++)
    delta2[k] = probs[k] - (k == label ? 1.0 : 0.0);
for (int j = 0; j < HIDDEN_SIZE; j++) {
    double grad = 0.0;
    for (int k = 0; k < OUTPUT_SIZE; k++)
        grad += delta2[k] * weight2[k][j];
    delta1[j] = (z1[j] > 0.0) ? grad : 0.0;
}
// SGD update — Step 5
```

Add one block at a time; run after each checkpoint. **Done** when average loss drops over epochs.
