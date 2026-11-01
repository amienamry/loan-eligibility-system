# plan.md: Personal Loan Eligibility System (Q1 Coding Plan)

> This is a plan only. It contains no C code. Follow the phases in order, compile after every phase, and tick the boxes as you go.

---

## 1. Goal

Write **ONE C source file (`.c`)** that:

1. Reads customer records in a loop until Customer ID = `0`
2. Validates income and commitments (re-prompts until valid)
3. Calculates the DTI ratio and decides Eligible / Not Eligible
4. Stores everything in arrays
5. Prints a summary (total, eligible, not eligible, average DTI)
6. Saves all records to `loanapplications.txt` and prints a "saved successfully" message

**Language rule:** the file must be C (`.c`), not C++. If you use Code::Blocks or Dev-C++, create a *C* project and not a C++ one.

---

## 2. Rubric → what the code must visibly show

| Rubric item | Marks | What the marker should be able to see in your code or output |
|---|---|---|
| Input & validation | 8 | ID, name, income and commitments are read. Out-of-range values are rejected with an error message and re-prompt. ID `0` ends the loop. |
| DTI & eligibility | 8 | The DTI formula is correct. The eligibility rule uses income ≥ 3000 **AND** DTI ≤ 40. Zero income is explicitly handled. |
| Functions | 8 | At least calculate DTI, determine eligibility and display result, each with proper parameters and return values (not global variables). |
| Records & summary | 8 | Arrays store all data. The four summary values are correct, and the average is computed safely. |
| File handling | 8 | `loanapplications.txt` is written with complete records (all 6 fields). `fopen` is checked and `fclose` is called. |
| Structure / modularity | 2 | Clear logic, correct loops and selections, small functions. |
| Readability / UI | 2 | Meaningful names, comments, a consistent style, a clean on-screen layout. |
| Robustness / testing | 4 | Compiles with no warnings, survives bad input (letters, negatives), covers boundary cases. |
| Documentation | 2 | Cover page, introduction, problem description, source code, screenshots, `.c` submitted separately. |

---

## 3. Constants (define once, use everywhere, with no magic numbers in the logic)

| Constant | Value | Used for |
|---|---|---|
| `MAX_CUSTOMERS` | 100 | Array size and loop guard |
| `MAX_NAME_LEN` | 50 | Name array width |
| `MIN_INCOME` / `MAX_INCOME` | 0 / 100000 | Income validation |
| `MIN_COMMITMENT` / `MAX_COMMITMENT` | 0 / 50000 | Commitments validation |
| `MIN_LOAN_INCOME` | 3000 | Eligibility rule |
| `MAX_DTI` | 40.0 | Eligibility rule |
| `FILE_NAME` | `"loanapplications.txt"` | File handling |
| `ELIGIBLE` / `NOT_ELIGIBLE` | 1 / 0 | Status flag, for readability |

---

## 4. Data storage

Use **parallel arrays**, where index `i` always refers to the same customer.

| Array | Type | Holds |
|---|---|---|
| `customerID[MAX_CUSTOMERS]` | int | Customer ID |
| `customerName[MAX_CUSTOMERS][MAX_NAME_LEN]` | char (2D) | Customer name |
| `monthlyIncome[MAX_CUSTOMERS]` | double | Monthly income (RM) |
| `commitments[MAX_CUSTOMERS]` | double | Total monthly commitments (RM) |
| `dtiRatio[MAX_CUSTOMERS]` | double | DTI % (0.0 when income = 0) |
| `eligibility[MAX_CUSTOMERS]` | int | 1 = Eligible, 0 = Not Eligible |

Local variables in `main`: `count` (number of customers stored), plus temporary variables for the record currently being entered.

**Tip:** have the summary function loop through the **arrays** to count eligible customers and add up the DTIs. That proves the arrays are really used and not just filled. Optionally cross-check against running counters while testing.

---

## 5. Functions

| # | Function | Purpose | Parameters | Returns |
|---|---|---|---|---|
| 1 | `clearInputBuffer` | Discard leftover characters after `scanf` | none | void |
| 2 | `getCustomerID` | Prompt for the ID and reject negatives or non-numbers. `0` means stop. | none | int |
| 3 | `getCustomerName` | Read a full name including spaces | name array, size | void |
| 4 | `getValidatedAmount` | Prompt, read, check range, loop until valid | prompt text, min, max | double |
| 5 | `calculateDTI` | `(commitments × 100) / income`. If income is 0, return 0.0 without dividing. | income, commitments | double |
| 6 | `determineEligibility` | Income ≥ 3000 **AND** DTI ≤ 40 gives Eligible, otherwise Not Eligible. Income 0 is always Not Eligible. | income, dti | int |
| 7 | `getStatusText` (helper) | Turn 1/0 into "Eligible" / "Not Eligible" | status flag | const char* |
| 8 | `displayResult` | Print one customer's details neatly. Show "N/A" for DTI when income is 0. | id, name, income, commitments, dti, status | void |
| 9 | `calculateAverageDTI` | total DTI ÷ number of customers, guarded against 0 customers | total DTI, count | double |
| 10 | `displaySummary` | Print the 4 summary values | arrays, count | void |
| 11 | `saveToFile` | Write all records to the file | arrays, count | int (1 = success, 0 = fail) |

**Rules for the functions**
- Pass values through parameters and return results, and avoid global variables.
- Each function does one job. `main` coordinates, and the functions do the work.
- Put a short comment block above each function (purpose, parameters, return). This also helps Question 2's documentation point.

---

## 6. Input-reading approach (decide now, and stay consistent)

| Option | How | Pros / cons |
|---|---|---|
| **A (simpler, course-level)** | `scanf` and check its return value. On failure, call `clearInputBuffer` and show an error. Call `clearInputBuffer` after every successful read too. | Easy. It accepts input like `12abc` as `12`. |
| **B (more robust)** | Read a whole line with `fgets`, then convert with `sscanf` or `strtod`/`strtol` and check for leftover characters. | Rejects `12abc` properly, but is slightly more work. |

For the **name**, read the whole line (`fgets`, or `scanf` with `%[^\n]` after clearing the buffer). A plain `%s` will break names like "Ahmad bin Ali". If you use `fgets`, strip the trailing newline.

---

## 7. Main program flow (pseudocode level)

1. Print the title banner.
2. Declare the arrays and `count = 0`.
3. **Loop:**
   1. If `count == MAX_CUSTOMERS`, print "capacity reached" and leave the loop.
   2. Get the customer ID. If it is `0`, leave the loop.
   3. Get the name.
   4. Get the income (range 0 to 100000).
   5. Get the commitments (range 0 to 50000).
   6. Calculate the DTI, then determine eligibility.
   7. Store all 6 values at index `count`.
   8. Display the result for this customer.
   9. `count++`
4. **After the loop:**
   - If `count == 0`: print "No customers processed". Skip the summary and the file, or write only the header (choose one and document it).
   - Otherwise: display the summary, call `saveToFile`, and print the success or failure message based on the return value.
5. Print a goodbye message.

---

## 8. Rules and pitfalls to remember

- **DTI boundary:** compute `commitments * 100.0 / income`, **multiplying first and then dividing**. This avoids floating-point error at exactly 40% (for example 2000 and 5000 must give exactly 40.0 and be Eligible).
- **Use `double`, not `float`,** for money and DTI.
- **Zero income:** never divide by zero. Store DTI as 0.0, display "N/A", classify Not Eligible, and write "N/A" in the file.
- **Average DTI:** the brief says total DTI ÷ total customers, so zero-income customers count in the denominator. State this assumption in your documentation.
- **Customer ID:** must be a positive integer. `0` stops the program, and negatives show an error and re-prompt.
- **Letters instead of numbers:** always check the `scanf` return value, otherwise you get an infinite loop.
- **Array overflow:** check `count` against `MAX_CUSTOMERS` before storing.
- **Name length:** limit the input to `MAX_NAME_LEN - 1` characters so it cannot overflow the array.
- **Printing `%`:** in `printf`, write `%%` to show a literal percent sign.
- **Optional extra:** reject a duplicate Customer ID (needs a small helper that loops through the array).

---

## 9. File output design (`loanapplications.txt`)

- Open with mode `"w"` (overwrite each run, so one run gives one clean file). Document this choice.
- Check whether `fopen` returned `NULL`, and print an error if it did.
- Write a header line once, then one line per customer.
- Use a consistent delimiter. If names could contain commas, use `|` or wrap the name in quotes.
- Call `fclose` when done.

Target layout:

```
CustomerID,CustomerName,MonthlyIncome,TotalCommitments,DTIRatio,Status
1001,Ahmad bin Ali,5000.00,1500.00,30.00,Eligible
1002,Siti Aminah,0.00,500.00,N/A,Not Eligible
```

---

## 10. Screen layout (design target)

```
==================================================
      PERSONAL LOAN ELIGIBILITY SYSTEM
==================================================
Enter Customer ID (0 to stop): 1001
Enter Customer Name: Ahmad bin Ali
Enter Monthly Income (RM0 - RM100000): -50
  [ERROR] Monthly income must be between RM0 and RM100000.
Enter Monthly Income (RM0 - RM100000): 5000
Enter Total Monthly Commitments (RM0 - RM50000): 1500

--------------------------------------------------
 Customer ID      : 1001
 Customer Name    : Ahmad bin Ali
 Monthly Income   : RM 5000.00
 Commitments      : RM 1500.00
 DTI Ratio        : 30.00%
 Loan Status      : ELIGIBLE
--------------------------------------------------

================== SUMMARY ==================
 Total customers processed : 4
 Total Eligible            : 2
 Total Not Eligible        : 2
 Average DTI ratio         : 30.00%
=============================================
 Customer records saved successfully to loanapplications.txt
```

Keep the same labels, widths and spacing on every screen. It helps the UI marks and makes your screenshots look tidy.

---

## 11. Build phases (compile and test after each one)

### Phase 0: Setup
- [x] Create the file (e.g. `loan_system.c`) and add a header comment (name, matric no., date, purpose)
- [x] Include the needed headers (`stdio.h`, `string.h`, and others only if you use them)
- [x] Add the constants from Section 3

### Phase 1: Skeleton
- [x] Write all function prototypes
- [x] Write function bodies
- [x] Compile with warnings on, e.g. `gcc -Wall -Wextra -std=c99 loan_system.c -o loan`
- **Done when:** it compiles with zero warnings.

### Phase 2: Input and validation
- [x] `clearInputBuffer`
- [x] `getValidatedAmount`: test with valid, below-min, above-max and letters
- [x] `getCustomerID` and `getCustomerName`
- **Done when:** a temporary test `main` accepts good input and rejects bad input without crashing or looping forever.

### Phase 3: Calculation and eligibility
- [x] `calculateDTI` including the zero-income guard
- [x] `determineEligibility`
- [x] Test the boundaries: (5000, 2000), (3000, 1200), (2999, 0), (0, 0)
- **Done when:** all boundary results match the test-case table.

### Phase 4: Main loop and arrays
- [x] Sentinel loop ending on ID `0`
- [x] Store each record in the arrays
- [x] Capacity check
- **Done when:** you can enter 3 or more customers and stop cleanly.

### Phase 5: Display result
- [x] `getStatusText` and `displayResult`
- [x] "N/A" for zero income
- **Done when:** every customer shows all 6 fields in the agreed layout.

### Phase 6: Summary
- [x] `calculateAverageDTI` with a zero-count guard
- [x] `displaySummary` using the arrays
- [x] The "No customers processed" case
- **Done when:** the summary matches your hand calculation.

### Phase 7: File handling
- [x] `saveToFile` with the `NULL` check and `fclose`
- [x] The success or failure message in `main`
- [ ] Open the `.txt` in Notepad and compare it to the screen
- **Done when:** the file holds every record with all 6 fields.

### Phase 8: Polish
- [x] Consistent naming (camelCase for variables and functions, UPPER_CASE for constants)
- [x] Comments above every function and non-obvious line
- [x] Remove debugging prints and unused variables
- [x] Consistent indentation and spacing
- [x] Final screen layout check

### Phase 9: Testing and screenshots
- [x] Run every test case in Section 12
- [ ] Take screenshots with captions (input and expected result)
- [ ] Save a screenshot of the output file in Notepad

Automated checks also verified the output file's six CSV fields, quoted names,
overwrite behavior, file-open failure handling, and the 100-customer limit.

---

## 12. Testing checklist

| # | Scenario | Income | Commitments | Expected |
|---|---|---|---|---|
| 1 | Normal eligible | 5000 | 1500 | 30.00%, Eligible |
| 2 | DTI exactly 40% | 5000 | 2000 | 40.00%, Eligible |
| 3 | DTI above 40% | 5000 | 2500 | 50.00%, Not Eligible |
| 4 | Income below 3000 | 2500 | 500 | 20.00%, Not Eligible |
| 5 | Income exactly 3000 | 3000 | 1200 | 40.00%, Eligible |
| 6 | Zero income | 0 | 500 | N/A, Not Eligible |
| 7 | Zero income and commitments | 0 | 0 | N/A, Not Eligible |
| 8 | Maximum values | 100000 | 50000 | 50.00%, Not Eligible |
| 9 | Zero commitments | 4000 | 0 | 0.00%, Eligible |
| 10 | Income negative (-100) | | | Error, re-prompt |
| 11 | Income too high (100001) | | | Error, re-prompt |
| 12 | Commitments negative (-50) | | | Error, re-prompt |
| 13 | Commitments too high (50001) | | | Error, re-prompt |
| 14 | Letters ("abc") | | | Error, re-prompt, no infinite loop |
| 15 | Negative Customer ID (-5) | | | Error, re-prompt |
| 16 | ID `0` immediately | | | "No customers processed", no crash |
| 17 | Several customers, then summary | cases 1–6 | | Totals and average match hand calculation |

**Extra checks**
- [x] A name with spaces is stored and printed correctly
- [x] A very long name does not crash the program
- [x] The output file matches what was shown on screen
- [x] Running the program twice overwrites the file cleanly (mode `"w"`)
- [x] The file-open failure message works (for example, test with a read-only or invalid path temporarily)

**Hand-check example:** cases 1, 2, 3, 6 have DTIs 30, 40, 50, 0, so the average is 120 ÷ 4 = **30.00%**.

---

## 13. Pre-submission checklist

- [x] Compiles with no errors **and** no warnings
- [ ] Source file has the `.c` extension and is submitted separately from the documentation
- [ ] The header comment is present, with your name and matric number
- [ ] All 9 rubric items in Section 2 are visibly satisfied
- [x] Any code adapted from external sources is acknowledged in a comment (no external code was adapted)
- [ ] Screenshots are numbered and captioned, and match the test-case numbers
- [ ] The documentation states these assumptions:
  - Zero-income DTI stored as 0.0 and shown as "N/A"
  - Zero-income customers are included in the average DTI denominator
  - The file is overwritten on each run
  - The behaviour when 0 customers are entered
- [ ] The full source code is pasted into the documentation in a monospaced font
