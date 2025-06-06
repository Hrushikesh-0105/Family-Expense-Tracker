# 💸 Family Expense Tracker (C, B-Trees)

A modular, CLI-based expense tracking system designed for families, built using C and an efficient **B-Tree** structure for scalable storage, fast lookup, and persistent records.

---

## 🚀 Features

- 🏠 **Multi-User Family Support:** Sign up/login, manage multiple users per family.
- 🌳 **Generic B-Tree-Based Storage:** All expenses are stored in a scalable B-Tree, supporting fast inserts, range queries, and retrieval.
- 🎯 **Function Pointer Abstractions:** One traversal function handles insert/search/delete operations using dynamic dispatch.
- 📊 **Analytics:** View monthly totals, top spending day, category-wise breakdowns, date-range filters, etc.
- 💾 **Persistent Storage:** All data is written to files (no DB needed).
- 📁 **Modular Codebase:** Clean separation into 10+ `.c/.h` files using `Makefile` build system.

---

## 🧠 Key Concepts & Highlights

- **Function Pointers & Abstraction:** Generic traversal logic accepts function pointers, making code reusable and decoupled.
- **Template-Like B-Trees in C:** Implemented B-Trees using `void*` to allow storage of arbitrary data types (like templates in C++).
- **Separation of Concerns:** Logic split across modules: `user.c`, `expense.c`, `family.c`, `file_ops.c`, `B_Tree.c`, etc.
- **Queue & Linked List:** Used for managing internal flows and buffers.
- **File I/O:** On-disk data persistence without any external libraries.

---

## 🛠️ Build Instructions

```bash
# Option 1: Using Make
make

# Option 2: Manual compilation
gcc linked_list.c B_Tree.c user.c family.c expense.c main.c queue.c file_ops.c -o project
```

---

## 🖥️ Usage

```bash
./project  # or ./a.exe on Windows
```

Use the CLI menu to:
- Sign up / log in
- Add users to family
- Add / update / delete expenses
- View detailed reports

---

## 📂 Project Structure

| File         | Purpose                               |
|--------------|----------------------------------------|
| `main.c`     | Entry point, controls app flow         |
| `B_Tree.c/h` | Generic B-Tree implementation          |
| `user.*`     | User-related logic (auth, display)     |
| `family.*`   | Family structure and operations        |
| `expense.*`  | Expense creation and analytics         |
| `file_ops.*` | File I/O and persistence               |
| `queue.*`    | Internal queue data structure          |
| `linked_list.*` | List utility for family members     |

---

## 🧰 Technologies Used

- Language: **C**
- Data Structures: **B-Trees, Queues, Linked Lists**
- Concepts: **Function Pointers, File I/O, CLI, Modular Design**

---
