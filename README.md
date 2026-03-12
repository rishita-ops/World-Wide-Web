# DSA — URL Validator

A C++ implementation that validates whether a given string is a **well-formed URL** by checking its protocol prefix, the presence of a dot, and the character composition of the remaining string. This problem is a practical exercise in **string parsing**, **character classification**, and **condition layering** — patterns that appear in input sanitization, web scraping, and compiler/interpreter front-ends. It also serves as a concrete example of where rule-based string validation starts to show its limits against the full complexity of real-world URL structure.

---

## Problem Statement

Given a string, determine whether it is a **valid URL** under the following rules:
- Starts with `http://` or `https://`
- Contains at least one `.` (dot)
- Every character after the prefix is alphanumeric, `-`, or `.`

**Example Input:**
```
https://www.example.com
```

**Example Output:**
```
valid
```

---

**Example Input:**
```
ftp://example.com
```

**Example Output:**
```
invalid
```

---

## The Code

```cpp
#include<bits/stdc++.h>
using namespace std;

int main()
{
    string s;
    cin >> s;

    if ((s.substr(0,7) == "http://" || s.substr(0,8) == "https://")
        && s.find('.') != string::npos)
    {
        bool valid = true;
        for (int i = 0; i < s.size(); i++)
        {
            if (!(isalnum(s[i]) || s[i] == '-' || s[i] == '.'
                || (i < 7 && s.substr(0,7) == "http://")
                || (i < 8 && s.substr(0,8) == "https://")))
            {
                valid = false;
                break;
            }
        }

        if (valid)
            cout << "valid";
        else
            cout << "invalid";
    }
    else
        cout << "invalid";

    return 0;
}
```

---

## How It Works — Three Layers of Validation

### Layer 1 — Protocol Prefix Check
```cpp
s.substr(0,7) == "http://" || s.substr(0,8) == "https://"
```
Checks that the string begins with a recognized protocol. Any string that doesn't start with `http://` or `https://` is immediately rejected without further processing.

### Layer 2 — Dot Presence Check
```cpp
s.find('.') != string::npos
```
`std::string::find` returns `string::npos` (a large sentinel value) when the character is not found. The condition confirms at least one `.` exists anywhere in the string — a minimal domain structure requirement.

### Layer 3 — Character Validation Loop
Every character is checked against an allowed set. Any character outside the set sets `valid = false` and breaks immediately.

---

## The Loop Condition — The Most Important Detail

The character validation condition inside the loop is:

```cpp
!(isalnum(s[i]) || s[i] == '-' || s[i] == '.'
  || (i < 7 && s.substr(0,7) == "http://")
  || (i < 8 && s.substr(0,8) == "https://"))
```

The last two sub-conditions are the subtle part:

```cpp
(i < 7 && s.substr(0,7) == "http://")
(i < 8 && s.substr(0,8) == "https://")
```

**What these do:** For positions `0` through `6` in an `http://` URL (or `0` through `7` in an `https://` URL), the condition evaluates to `true` — meaning every character in the prefix is **unconditionally allowed**, regardless of what it actually is.

**Why this is needed:** The prefix characters `:` and `/` are neither alphanumeric, nor `-`, nor `.`. Without this exemption, even a perfectly valid URL like `http://example.com` would fail because `:` and `/` at positions 4, 5, 6 would be rejected.

**The trade-off:** This approach whitelists the prefix positions implicitly rather than explicitly adding `':'` and `'/'` to the allowed character set. It works correctly but is harder to read and reason about than a more explicit approach would be.

**A cleaner equivalent:**
```cpp
// Validate prefix separately, then validate the rest explicitly
string rest = s.substr(prefix_length);
for (char c : rest) {
    if (!(isalnum(c) || c == '-' || c == '.')) {
        valid = false;
        break;
    }
}
```

---

## Algorithm (Pseudocode)

```
read s

if s does not start with "http://" or "https://":
    print "invalid"
    exit

if s contains no '.':
    print "invalid"
    exit

valid ← true
for each character s[i]:
    if s[i] is not (alphanumeric OR '-' OR '.' OR within prefix range):
        valid ← false
        break

print "valid" if valid else "invalid"
```

---

## Dry Run

**Input:** `https://www.example.com`

**Layer 1:** `s.substr(0,8) == "https://"` → ✅
**Layer 2:** `s.find('.')` → found at index 11 → ✅

**Layer 3 — Character scan:**

| `i` | `s[i]` | alphanumeric / `-` / `.`? | In prefix range (`i<8`)? | Allowed? |
|-----|--------|--------------------------|--------------------------|----------|
| 0-7 | `https://` | `:` and `/` fail char check | ✅ `i < 8` | ✅ |
| 8   | `w`    | ✅ alphanumeric            | —                        | ✅ |
| 9   | `w`    | ✅ alphanumeric            | —                        | ✅ |
| 10  | `w`    | ✅ alphanumeric            | —                        | ✅ |
| 11  | `.`    | ✅ dot                     | —                        | ✅ |
| ... | `example.com` | ✅ all alphanumeric / dot | —                   | ✅ |

**Output:** `valid` ✅

---

**Input:** `https://www.example.com/path`

| Character | `s[i]` | Allowed? |
|-----------|--------|----------|
| ...       | `example.com` | ✅ |
| 23        | `/`    | Not alphanumeric, not `-`, not `.`, not in prefix range | ❌ → `invalid` |

**Output:** `invalid` ⚠️ *(despite being a perfectly valid real-world URL)*

---

## Complexity Analysis

| Metric | Complexity |
|--------|------------|
| Time   | **O(n)** — single pass through the string; `substr` in prefix check is O(1) for fixed lengths |
| Space  | **O(1)** — no auxiliary data structures; only a boolean flag and loop variable |

> **Repeated `substr` inside the loop:** `s.substr(0,7)` and `s.substr(0,8)` are called on every iteration of the loop — these create new string objects each time, technically O(k) per call. Since k is fixed (7 or 8), this is O(1) per iteration in practice, but it is inefficient style. Precomputing `bool isHttp = (s.substr(0,7) == "http://")` before the loop would be cleaner.

---

## What This Validator Accepts and Rejects

### ✅ Accepted (Valid by this code)

| URL | Reason |
|-----|--------|
| `http://example.com` | Protocol + dot + clean domain |
| `https://www.google.com` | Protocol + dot + alphanumeric domain |
| `https://my-site.com` | Hyphen allowed |
| `http://sub.domain.co.uk` | Multiple dots allowed |

### ❌ Rejected (Invalid by this code, but valid in reality)

| URL | Rejected Because |
|-----|-----------------|
| `https://example.com/path` | `/` after prefix not allowed |
| `https://example.com?q=search` | `?`, `=` not in allowed set |
| `https://example.com#section` | `#` not in allowed set |
| `http://example.com:8080` | `:` after prefix not in allowed set |
| `https://user:pass@example.com` | `@`, `:` not in allowed set |
| `https://xn--nxasmq6b.com` | `--` pattern technically allowed but edge cases exist |

### ❌ Rejected (Correctly invalid)

| URL | Reason |
|-----|--------|
| `ftp://example.com` | Wrong protocol |
| `http://nodothere` | No `.` found |
| `https://exam ple.com` | Space not in allowed set (also, `cin` would truncate here) |
| `randomstring` | No recognized protocol |

---

## Edge Cases

| Input | Output | Notes |
|-------|--------|-------|
| `http://.` | `valid` ⚠️ | Passes all three checks — not a real domain |
| `https://.com` | `valid` ⚠️ | No hostname before dot — structurally invalid but accepted |
| `http://a.b` | `valid` ✅ | Minimal valid-looking URL |
| `http://` | `invalid` ✅ | No dot present |
| `https://` | `invalid` ✅ | No dot present |
| Empty string | Undefined behavior ⚠️ | `substr(0,7)` on a string shorter than 7 returns the whole string — no crash, but behavior depends on string content |

---

## A Note on `cin >> s`

The code uses `cin >> s`, which reads a single whitespace-delimited token. URLs with spaces (malformed as they are) would be silently truncated at the first space. For reading full lines including spaces, `getline(cin, s)` is the correct approach — though well-formed URLs should never contain unencoded spaces.

---

## A Note on `#include <bits/stdc++.h>`

The only headers actually needed for this program are:

```cpp
#include <iostream>   // cin, cout
#include <string>     // string, substr, find
```

`bits/stdc++.h` is a GCC-specific convenience header — practical in competitive programming, not recommended in production or interview code at product companies.

---

## Repository Structure

```
DSA-URL-Validator/
│
├── url_validator.cpp     # Main C++ implementation
└── README.md             # Project documentation
```

---

## How to Compile and Run

**Prerequisites:** GCC / G++

```bash
# Clone the repository
git clone https://github.com/rishita-ops/DSA-URL-Validator.git
cd DSA-URL-Validator

# Compile
g++ url_validator.cpp -o url_validator

# Run
./url_validator
```

**On Windows:**
```bash
g++ url_validator.cpp -o url_validator.exe
url_validator.exe
```

---

## Key Concepts Covered

- **`std::string::substr`** — extracting a fixed-length prefix for protocol detection
- **`std::string::find`** — searching for a character and comparing against `string::npos`
- **`isalnum`** — standard C character classification for alphanumeric check
- **Prefix range whitelisting in loop** — using positional bounds to implicitly allow protocol characters
- **Guard clause / early exit** — rejecting invalid inputs at the earliest possible check
- **`string::npos`** — the sentinel return value of `find` when the character is not present
- **Layered validation** — decomposing a complex check into ordered, independent conditions

---

## Why Real URL Validation is Hard

This implementation captures the spirit of URL validation cleanly for simple cases, but real URLs are governed by **RFC 3986**, which defines a far richer grammar:

```
URI = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
```

A production-grade URL validator must handle:

| Component | Example | Not handled here |
|-----------|---------|-----------------|
| Path | `/api/v1/users` | `/` rejected after prefix |
| Query string | `?name=john&age=30` | `?`, `=`, `&` rejected |
| Fragment | `#section-2` | `#` rejected |
| Port | `:8080` | `:` rejected after prefix |
| Percent encoding | `%20` for space | `%` rejected |
| IPv6 | `[::1]` | `[`, `]` rejected |
| Internationalized domains | `münchen.de` | Non-ASCII rejected by `isalnum` |

For production use, regex-based validation or a dedicated URI parsing library is the correct tool. This implementation is the right starting point for understanding the structural components of a URL before tackling the full specification.

---

## Contributing

Contributions are welcome. Consider adding:
- A **regex-based version** using `std::regex` for richer pattern matching
- Support for **paths, query strings, and fragments**
- Support for **port numbers** in the URL
- Input validation for edge cases like empty strings and very short inputs
- Implementations in Python, Java, or JavaScript

```bash
git checkout -b feature/your-feature
git commit -m "Add: your feature description"
git push origin feature/your-feature
# Then open a Pull Request
```

---

## License

This project is open-source and available under the [MIT License](LICENSE).

---

*Part of a structured DSA practice series — fundamentals, done right.*
