# Bug Report: Battleship P2P Implementation

## Critical Bugs Found

### Bug 1: Off-by-one error in bounds checking (func.c:399, 457)
**Location:** `func.c:399` and `func.c:457`

**Issue:** After decrementing row and column (making them 0-indexed), the bounds check allows values up to 10:
```c
} while ((row < 0 || row > 10) || (colum < 0 || colum > 10));
```

**Failure scenario:** User enters "11" for row or column. After decrement, it becomes 10, which passes the check `row > 10` (10 is not > 10). Then `board[row][colum]` with row=10 or colum=10 causes array out-of-bounds access (index 10 is invalid for a 10x10 array indexed 0-9).

**Correct check should be:** `row >= 10 || colum >= 10` or `row > 9 || colum > 9`

---

### Bug 2: Off-by-one error in ship placement validation loops (func.c:136, 182, 205)
**Location:** `func.c:136`, `func.c:182`, `func.c:205`

**Issue:** The validation loops check one cell too many:
- Line 136: `for (int i = IpoR; i <= (IpoR+dimShip); i++)` - iterates `dimShip+1` times
- Line 182: Same issue for '<' case
- Line 205: `for (int i = IpoC; i >= (IpoC-dimShip); i--)` - iterates `dimShip+1` times

**Failure scenario:** When placing a 5-cell ship, the validation checks 6 cells instead of 5. If the 6th cell happens to be occupied, a valid 5-cell ship placement is incorrectly rejected. Conversely, if the 6th cell is beyond the board boundary, it could cause out-of-bounds access.

---

### Bug 3: Potential buffer over-read in up-direction ship placement (func.c:159)
**Location:** `func.c:159`

**Issue:** In the 'V' (up) case for `place_other()`:
```c
for (int i = IpoR; i >= (IpoR-dimShip); i--)
```
For dimShip=5 and IpoR=4, this iterates i=4,3,2,1,0,-1. The iteration to i=-1 causes a buffer over-read.

**Failure scenario:** When placing a ship near the top edge (row 4 or lower), the validation loop reads from invalid memory location at row -1.

---

### Bug 4: Turn counter logic causes players to defend when they should attack (func.c:373-508)
**Location:** `func.c:381-497`

**Issue:** The turn-based logic is fundamentally broken. The code uses a single `turno` counter to determine both attacker and defender, but the logic is inverted:
- Turn 1: Player 1 attacks ✓
- Turn 2: Player 1 defends ✗ (Player 2 should be defending here!)
- Turn 3: Player 2 attacks ✓
- Turn 4: Player 2 defends ✗ (Player 1 should be defending here!)

The defend phases should automatically happen after receiving an attack, not be controlled by a separate turn counter.

**Failure scenario:** Player 1 defends on turn 2, but Player 2's attack from turn 1 was never processed correctly. The game state becomes desynchronized, and players defend when they should be attacking.

---

### Bug 5: Missing input buffer clear on failed scanf (func.c:394-398, 451-457)
**Location:** `func.c:394-398` and `func.c:451-457`

**Issue:** If the user enters non-numeric input (e.g., "abc"), `scanf("%d", &row)` fails but doesn't clear the input buffer. The loop condition then re-checks the same failed input.

**Failure scenario:** User types "abc" instead of a number. `scanf` fails, `row` remains 0, and the loop accepts it as valid input (0 is in range 0-9 after decrementing from 1). The user's invalid input is silently accepted.

---

### Bug 6: Inconsistent turn increment in defend phases (func.c:432-436, 491-496)
**Location:** `func.c:432-436` and `func.c:491-496`

**Issue:** In the defend phases, `turno++` only executes if `send_status > 0`:
```c
if (status_send > 0)
{
    turno++;
}
```

But in attack phases, `turno++` always executes. This can cause the game to loop infinitely if `send_status` fails.

**Failure scenario:** If `send_status` returns 0 or negative (network error), `turno` doesn't increment, and the same player gets stuck in a loop defending repeatedly.

---

### Bug 7: Missing NULL check for fgets (main.c:53-54)
**Location:** `main.c:53-54`

**Issue:** 
```c
fgets(address, sizeof(address), stdin);
address[strcspn(address, "\n")] = '\0';
```

If `fgets` returns NULL (EOF or error), `strcspn` is called on a NULL pointer.

**Failure scenario:** User sends EOF (Ctrl+D on Unix) or input error occurs. Program crashes with segmentation fault.

---

### Bug 8: Incorrect TOTAL_CELLS constant name (functions.h:27)
**Location:** `functions.h:27`

**Issue:** `#define TOTAL_CELLS 16` is misleading. The comment says it's total ship cells, but the name suggests it should be board cells (100). This is a naming/documentation issue.

**Failure scenario:** A developer unfamiliar with the code might incorrectly modify this constant, thinking it represents the full board size.

---

## Suggested Fixes

### Fix 1: Correct bounds checking (func.c:399, 457)
**Change:**
```c
// From:
} while ((row < 0 || row > 10) || (colum < 0 || colum > 10));

// To:
} while ((row < 0 || row >= 10) || (colum < 0 || colum >= 10));
```

### Fix 2: Correct ship validation loop bounds (func.c:136, 182, 205)
**Change:**
```c
// From (line 136):
for (int i = IpoR; i <= (IpoR+dimShip); i++)

// To:
for (int i = IpoR; i < (IpoR+dimShip); i++)
```

Apply the same fix to line 182 ('<' case) and line 205 ('>' case).

### Fix 3: Fix buffer over-read in up-direction (func.c:159)
**Change:**
```c
// From:
for (int i = IpoR; i >= (IpoR-dimShip); i--)

// To:
for (int i = IpoR; i >= (IpoR-dimShip+1); i--)
```

This ensures we never read beyond row 0.

### Fix 4: Redesign turn-based logic (func.c:373-508)
The entire turn system needs restructuring. The defend phase should happen immediately after receiving an attack, not be controlled by a turn counter. Consider using a state machine:

```c
// Simplified approach:
// 1. Player 1 sends attack coordinates
// 2. Player 2 receives, validates, sends hit/miss status
// 3. Player 2 sends attack coordinates  
// 4. Player 1 receives, validates, sends hit/miss status
// 5. Repeat until one player's ships are sunk
```

Or use a simpler turn counter that just alternates who sends their attack:
```c
// Turn 1, 3, 5...: Player 1 attacks, Player 2 defends
// Turn 2, 4, 6...: Player 2 attacks, Player 1 defends
```

### Fix 5: Clear input buffer on failed scanf (func.c:391-399, 449-457)
**Change:**
```c
do {
    printf("\nInsert the row (1 - 10 ): \n>");
    if (scanf("%d", &row) != 1) {
        // Clear input buffer
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        row = -1;  // Force loop to continue
        continue;
    }
    row--;
    printf("\nInsert the colum ( 1 - 10 ): \n>");
    if (scanf("%d", &colum) != 1) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        colum = -1;
        continue;
    }
    colum--;
} while ((row < 0 || row >= 10) || (colum < 0 || colum >= 10));
```

### Fix 6: Always increment turn counter in defend phases (func.c:432-436, 491-496)
**Change:**
```c
// From:
if (status_send > 0)
{
    turno++;
}

// To:
turno++;  // Always increment to move to next phase
if (status_send <= 0)
{
    perror("Network error in defend phase");
    return FAIL;  // Or handle error appropriately
}
```

### Fix 7: Add NULL check for fgets (main.c:53-54)
**Change:**
```c
// From:
fgets(address, sizeof(address), stdin);
address[strcspn(address, "\n")] = '\0';

// To:
if (fgets(address, sizeof(address), stdin) == NULL) {
    fprintf(stderr, "Error reading IP address\n");
    return FAIL;
}
address[strcspn(address, "\n")] = '\0';
```

### Fix 8: Rename constant for clarity (functions.h:27)
**Change:**
```c
// From:
#define TOTAL_CELLS 16

// To:
#define TOTAL_SHIP_CELLS 16
```

Then update all references from `celleRimanentiG1 = TOTAL_CELLS` to `celleRimanentiG1 = TOTAL_SHIP_CELLS`, etc.

---

## Summary

| Bug # | Severity | File | Line | Description |
|-------|----------|------|------|-------------|
| 1 | Critical | func.c | 399, 457 | Off-by-one bounds check allowing index 10 |
| 2 | Medium | func.c | 136, 182, 205 | Validation checks 1 extra cell |
| 3 | High | func.c | 159 | Potential buffer over-read (row -1) |
| 4 | Critical | func.c | 381-497 | Turn logic causes wrong player to defend |
| 5 | Medium | func.c | 394-398, 451-457 | No input buffer clear on failed scanf |
| 6 | Medium | func.c | 432-436, 491-496 | Inconsistent turn increment |
| 7 | Medium | main.c | 53-54 | Missing NULL check for fgets |
| 8 | Low | functions.h | 27 | Misleading constant name |