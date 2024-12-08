# cztodoby

C++ compile-time bomb (Fails compilation after a specified date/time)

Technical debt is inevitable, but ignoring it shouldn't be.
To encourage timely action, **cztodoby** provides compile-time bombs: macros that fail compilation after a specified date and time.
These serve as helpful reminders for tasks or deadlines, ensuring important updates aren't overlooked.

With **cztodoby**, you can embed deadlines directly into your code.
Once the compilation timestamp exceeds the specified timestamp, compilation will fail.
This is perfect for teams looking to set deadlines for code reviews, updates, or even personal reminders.

E.g:

```cpp
// Trying to compile after Dec 9th, 2024, at 4pm, will fail.
CZ_COMPILE_TIMEBOMB("Dec 09 2024 16:00:00", "Implement this function before finishing for the day.");
```

If you don't want a custom message you can simply use:

```cpp
CZ_TODO_BY("Dec 09 2024 16:00:00");
```

If the time is not specified, it assumes `00:00:00`:

```cpp
CZ_TODO_BY("Dec 09 2024");  // Fails as once the clock turns Dec 9th, 2024.
```

# Considerations

* **Triggered only during compilation**:
	* Compile-time bombs activate when the code is compiled. If the file containing the macro is not recompiled (e.g., due to incremental builds), the bomb will not trigger until a full rebuild.
* **Local vs CI Builds**:
    * **Local development**: Incremental builds may delay the trigger until you modify the file or do a full rebuild.
    * **Automated builds (e.g., CI/CD)** : Systems like GitHub Actions or other CI pipelines typically perform full rebuilds, ensuring that time bombs are checked regularly.

# How to use

* Ensure your project uses **C++20 or higher**.
* Include the `TodoBy.h` header in your code.
* Use the provided macros (`CZ_COMPILE_TIMEBOMB` or `CZ_TODO_BY`) where needed.

Depending on your team's preferences, you might want time bombs to trigger only for specific build types.
By default, they are enabled for all builds. To disable them for certain builds, define the `CZTODOBY_ENABLED` macro as `0` in the corresponding build configuration(s).

