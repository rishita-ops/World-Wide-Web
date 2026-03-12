# DSA — URL Validator (Extended)

An improved C++ implementation of URL validation that addresses the key structural and functional limitations of the basic approach. This version cleanly separates the **protocol prefix** from the **rest of the URL**, validates each independently, and supports a significantly richer character set — covering paths, query strings, fragments, port numbers, and encoded characters. It is a direct demonstration of how incremental refinement of a rule-based validator looks in practice.

---

## Problem Statement

Given a string entered by the user, determine whether it is a **valid URL** under these rules:
- Starts with `http://` or `https://`
- Contains at least one `.` in the portion after the protocol
- The portion after the protocol is non-empty
- Every character in the portion after the protocol belongs to the permitted URL character set

**Example Input:**
```
Enter the URL : https://www.example.com/path?q=hello&lang=en
```

**Example Output:**
```
valid
```

---

**Example Input:**
```
Enter the URL : ftp://example.com
```

**Example Output:**
```
invalid
```

---

## The Code

```cpp
#include <bits/stdc++.h>
using namespace std;

int main()
{
    string s;
    cout << "Enter the URL : ";
    cin >> s;

    string prefix;
    int start;

    if (s.substr(0, 7) == "http://")
    {
        prefix = "https://";
        start = 7;
    }
    else if (s.substr(0, 8) == "https://")
    {
        prefix = "https://";
        start = 8;
    }
    else
    {
        cout << "invalid";
        return 0;
    }

    string rest = s.substr(start);

    if (rest.find('.') == string::npos || rest.empty())
    {
        cout << "invalid";
        return 0;
    }

    bool valid = true;
    for (char c : rest)
    {
        if (!(isalnum(c) || c == '-' || c == '.' || c == '/'  ||
              c == '_'   || c == '?' || c == '=' || c == '&'  ||
              c == '#'   || c == '%' || c == '+' || c == ':'  || c == '@'))
        {
            valid = false;
            break;
        }
    }

    cout << (valid ? "valid" : "invalid");
    return 0;
}
```

---

## Architectural Improvement Over the Basic Version

This implementation fixes the most significant structural issue from the previous version — the **prefix range trick inside the validation loop**. That approach used positional bounds (`i < 7`) to silently whitelist prefix characters, making the logic opaque and hard to reason about.

Here the fix is clean and explicit:

```cpp
string rest = s.substr(start);   // strip the prefix entirely
for (char c : rest) { ... }      // validate only what comes after
```

By separating prefix extraction from body validation, the two concerns are **completely decoupled** — each is independently readable, testable, and modifiable.

| Aspect | Basic Version | This Version |
|--------|--------------|--------------|
| Prefix handling | Whitelisted by positional bounds in loop | Extracted upfront via `substr(start)` |
| Loop logic | Opaque — prefix characters silently allowed | Transparent — loop only sees post-prefix content |
| `rest` isolation | No — full string iterated | Yes — `rest` is a clean independent variable |
| Allowed characters | `alnum`, `-`, `.` only | Full RFC-aligned set (see below) |
| Readability | Requires careful analysis | Self-evident |

---

## How It Works — Three Clean Stages

### Stage 1 — Protocol Detection and `start` Assignment

```cpp
if (s.substr(0, 7) == "http://")  { start = 7; }
else if (s.substr(0, 8) == "https://") { start = 8; }
else { cout << "invalid"; return 0; }
```

The variable `start` records where the protocol ends and the actual URL body begins. The prefix is detected, its length is stored, and everything else is handled through `start`.

---

### Stage 2 — Rest Extraction and Structural Check

```cpp
string rest = s.substr(start);

if (rest.find('.') == string::npos || rest.empty())
{
    cout << "invalid";
    return 0;
}
```

`rest` is the URL body — everything after `http://` or `https://`. Two structural requirements are checked:

- **`rest.empty()`** — rejects bare `http://` or `https://` with nothing following
- **`rest.find('.') == string::npos`** — rejects URLs with no dot (no domain structure)

Note the **order of the conditions**: `rest.find('.')` is evaluated before `rest.empty()` in the `||` expression — but since `empty()` is cheaper, swapping their order to check `empty()` first would be a minor efficiency improvement:

```cpp
if (rest.empty() || rest.find('.') == string::npos)   // cheaper check first
```

---

### Stage 3 — Character Validation

```cpp
for (char c : rest)
{
    if (!(isalnum(c) || c == '-' || c == '.' || c == '/'  ||
          c == '_'   || c == '?' || c == '=' || c == '&'  ||
          c == '#'   || c == '%' || c == '+' || c == ':'  || c == '@'))
    {
        valid = false;
        break;
    }
}
```

Every character in `rest` is tested against the allowed set. The **range-based for loop** (`for (char c : rest)`) is cleaner and more idiomatic C++11 than an index-based loop for character iteration.

---

## The Allowed Character Set — What Each Character Covers

| Character(s) | URL Role |
|--------------|----------|
| `isalnum` (a-z, A-Z, 0-9) | Domain names, path segments, query values |
| `-` | Hyphenated domain names (`my-site.com`) |
| `.` | Domain separators (`www.example.co.uk`) |
| `/` | Path separators (`/api/v1/users`) |
| `_` | Underscores in paths or parameters |
| `?` | Query string start (`?q=search`) |
| `=` | Query key-value separator (`q=hello`) |
| `&` | Query parameter separator (`a=1&b=2`) |
| `#` | Fragment identifier (`#section-2`) |
| `%` | Percent-encoding prefix (`%20` for space) |
| `+` | Space encoding in query strings (`hello+world`) |
| `:` | Port separator (`example.com:8080`) |
| `@` | User credentials separator (`user@host`) |

This set covers the majority of characters found in real-world URLs for everyday use cases.

---

## Algorithm (Pseudocode)

```
read s

if s starts with "http://":
    start ← 7
else if s starts with "https://":
    start ← 8
else:
    print "invalid"
    exit

rest ← s[start..]

if rest is empty OR rest has no '.':
    print "invalid"
    exit

valid ← true
for each character c in rest:
    if c not in allowed set:
        valid ← false
        break

print "valid" if valid else "invalid"
```

---

## Dry Run

**Input:** `https://www.example.com/search?q=dsa&lang=en#results`

**Stage 1:**
```
s.substr(0,8) == "https://" → ✅   start = 8
```

**Stage 2:**
```
rest = "www.example.com/search?q=dsa&lang=en#results"
rest.empty()             → false ✅
rest.find('.')           → 3 (found) ✅
```

**Stage 3 — Character scan:**

| Character | Allowed By |
|-----------|------------|
| `w`, `w`, `w` | `isalnum` |
| `.` | `.` literal |
| `example` | `isalnum` |
| `.` | `.` literal |
| `com` | `isalnum` |
| `/` | `/` literal |
| `search` | `isalnum` |
| `?` | `?` literal |
| `q`, `=`, `dsa` | `isalnum`, `=` literal |
| `&` | `&` literal |
| `lang=en` | `isalnum`, `=` literal |
| `#` | `#` literal |
| `results` | `isalnum` |

All characters pass → **Output:** `valid` ✅

---

**Input:** `https://example.com/path with spaces`

```
rest = "example.com/path with spaces"
' ' (space) → not in allowed set → valid = false
```

**Output:** `invalid` ✅

---

## Complexity Analysis

| Metric | Complexity |
|--------|------------|
| Time   | **O(n)** — single pass through `rest`; prefix checks are O(1) for fixed lengths |
| Space  | **O(n)** — `rest` is a new string of length `n - start`; this is the only allocation |

> **Optimization note:** `string rest = s.substr(start)` allocates a new string. For large inputs, this can be avoided by iterating from index `start` directly: `for (int i = start; i < s.size(); i++)`. This keeps space at O(1) while maintaining the same clean separation of prefix and body logic.

---

## What This Validator Now Accepts vs. the Basic Version

| URL | Basic Version | This Version |
|-----|--------------|--------------|
| `https://example.com` | ✅ valid | ✅ valid |
| `https://example.com/path` | ❌ invalid (/ rejected) | ✅ valid |
| `https://example.com?q=search` | ❌ invalid (? = rejected) | ✅ valid |
| `https://example.com#section` | ❌ invalid (# rejected) | ✅ valid |
| `https://example.com:8080` | ❌ invalid (: rejected) | ✅ valid |
| `https://user@example.com` | ❌ invalid (@ rejected) | ✅ valid |
| `https://example.com/a%20b` | ❌ invalid (% rejected) | ✅ valid |
| `ftp://example.com` | ❌ invalid | ❌ invalid ✅ |
| `https://nodot` | ❌ invalid | ❌ invalid ✅ |

---

## Remaining Limitations

Despite the significant improvements, some real-world URL patterns are still not covered:

| Scenario | Status | Reason |
|----------|--------|--------|
| `https://example.com` with Unicode domain | ❌ Rejected | Non-ASCII characters fail `isalnum` |
| `%` followed by invalid hex (e.g., `%GG`) | ✅ Accepted ⚠️ | `%` is allowed but encoding validity is not checked |
| Dot at start of rest (`https://.com`) | ✅ Accepted ⚠️ | Dot presence is checked, not position |
| Dot at end (`https://example.`) | ✅ Accepted ⚠️ | Trailing dot not validated |
| Consecutive dots (`https://exam..ple.com`) | ✅ Accepted ⚠️ | No structural domain validation |
| IP addresses (`https://192.168.1.1`) | ✅ Accepted ✅ | Digits and dots are both allowed |
| IPv6 (`https://[::1]`) | ❌ Rejected | `[` and `]` not in allowed set |
| Spaces in URL | ❌ Rejected ✅ | Correctly invalid (also truncated by `cin`) |

---

## Edge Cases

| Input | Output | Notes |
|-------|--------|-------|
| `http://` | `invalid` ✅ | `rest.empty()` catches this |
| `https://` | `invalid` ✅ | `rest.empty()` catches this |
| `http://nodot` | `invalid` ✅ | `rest.find('.')` catches this |
| `http://a.b` | `valid` ✅ | Minimal valid-looking URL |
| `https://.com` | `valid` ⚠️ | No hostname before dot — not structurally valid |
| `http://a.b<script>` | `invalid` ✅ | `<` and `>` not in allowed set |
| Empty string | Undefined ⚠️ | `substr(0,7)` on empty string returns `""` — no crash, but no prompt either |

---

## A Note on `#include <bits/stdc++.h>`

The only standard headers needed here are:

```cpp
#include <iostream>   // cin, cout
#include <string>     // string, substr, find, empty
#include <cctype>     // isalnum
```

`bits/stdc++.h` includes the entire C++ standard library — a GCC-specific convenience used widely in competitive programming but flagged in production and interview code at product companies for its portability and compile-time cost.

---

## Repository Structure

```
DSA-URL-Validator-Extended/
│
├── url_validator.cpp     # Main C++ implementation
└── README.md             # Project documentation
```

---

## How to Compile and Run

**Prerequisites:** GCC / G++

```bash
# Clone the repository
git clone https://github.com/rishita-ops/DSA-URL-Validator-Extended.git
cd DSA-URL-Validator-Extended

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

- **Prefix extraction via `substr(start)`** — cleanly separating protocol from body before validation
- **Early return pattern** — rejecting invalid inputs at the earliest possible stage before any loop runs
- **`string::npos` sentinel** — the return value of `find` when the target character is absent
- **Range-based for loop on string** — idiomatic C++11 character iteration
- **Allowed character set design** — mapping each permitted character to its URL role
- **Condition order in `||` expressions** — checking cheaper conditions first
- **`substr` allocation trade-off** — clarity vs. memory when extracting `rest`
- **`bits/stdc++.h`** — competitive programming convenience vs. portable production headers

---

## Why URL Validation Matters in DSA and Beyond

| Application | Connection |
|-------------|------------|
| Web scraping | Every URL must be validated before a fetch request is made |
| Form input sanitization | Preventing malformed or malicious URLs from reaching a backend |
| Compiler / interpreter front-ends | Lexing and tokenizing structured input uses the same character-classification approach |
| Regex engines | This rule-based approach is essentially a hand-written subset of what `std::regex` does |
| RFC 3986 (URI Standard) | The formal grammar this implementation approximates — the natural next step for production validators |
| LeetCode string problems | Character classification, prefix detection, and guard clauses appear in dozens of string problems |

---

## Contributing

Contributions are welcome. Consider adding:
- **Percent-encoding validation** — verify that `%` is always followed by exactly two hex digits
- **Domain structure validation** — ensure no leading/trailing dots, no consecutive dots
- **IPv6 support** — allow `[` and `]` in the right context
- **`getline` support** — handle URLs with spaces (though these are inherently malformed)
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
