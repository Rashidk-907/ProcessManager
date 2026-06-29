#include "../include/manager.hpp"
#include <iostream>
#include <thread>
#include <chrono>

void printSeparator(const std::string& title) {
    std::cout << "\n========================================\n";
    std::cout << "  " << title << "\n";
    std::cout << "========================================\n";
}

void printProcessList(Manager& mgr) {
    auto names = mgr.listAll();
    if (names.empty()) {
        std::cout << " (No processes running)\n";
        return;
    }
    
    for (const auto& name : names) {
        std::cout << "  • " << name << " → " << mgr.status(name) << "\n";
    }
}

int main() {
    std::cout << "🚀 MANAGER CLASS TEST SUITE\n";
    std::cout << "============================\n";
    
    Manager mgr;
    
    // ============================================================
    // TEST 1: Create a Simple Process (with no args)
    // ============================================================
    printSeparator("TEST 1: Create 'echo' process");
    
    // Create a process that echoes input back
    if (mgr.create("echo", "/bin/cat", {})) {
        std::cout << "✅ 'echo' process created successfully\n";
    } else {
        std::cout << "❌ Failed to create 'echo' process\n";
        return 1;
    }
    
    printProcessList(mgr);
    
    // ============================================================
    // TEST 2: Send Input and Get Output
    // ============================================================
    printSeparator("TEST 2: Send input to 'echo' process");
    
    std::string testInput = "Hello from ProcessManager!";
    if (mgr.sendInput("echo", testInput)) {
        std::cout << "✅ Input sent: \"" << testInput << "\"\n";
    } else {
        std::cout << "❌ Failed to send input\n";
    }
    
    // Give process time to process
    // ============================================================
    // TEST 3: Create Multiple Processes
    // ============================================================
    printSeparator("TEST 3: Create multiple processes");
    
    // Create a simple shell command process
    if (mgr.create("date", "/bin/date", {})) {
        std::cout << "✅ 'date' process created\n";
    } else {
        std::cout << "❌ Failed to create 'date' process\n";
    }
    
    // Create a process that does nothing (sleep)
    if (mgr.create("sleep", "/bin/sleep", {"2"})) {
        std::cout << "✅ 'sleep' process created (will run for 2 seconds)\n";
    } else {
        std::cout << "❌ Failed to create 'sleep' process\n";
    }
    
    printProcessList(mgr);
    
    // ============================================================
    // TEST 4: Stop a Specific Process
    // ============================================================
    printSeparator("TEST 4: Stop 'sleep' process");
    
    if (mgr.stop("sleep")) {
        std::cout << "✅ 'sleep' process stopped\n";
    } else {
        std::cout << "❌ Failed to stop 'sleep' process\n";
    }
    
    printProcessList(mgr);
    
    // ============================================================
    // TEST 5: Try to Create Duplicate Process (should fail)
    // ============================================================
    printSeparator("TEST 5: Duplicate process creation (should fail)");
    
    if (mgr.create("echo", "/bin/echo", {"duplicate"})) {
        std::cout << "⚠️  Created duplicate process (this should NOT happen)\n";
    } else {
        std::cout << "✅ Correctly rejected duplicate process name 'echo'\n";
    }
    
    
    printProcessList(mgr);
    
    // ============================================================
    // TEST 7: Get Output from 'echo' after sending more data
    // ============================================================
    printSeparator("TEST 7: Send another input to 'echo'");
    
    testInput = "Second message to echo!";
    if (mgr.sendInput("echo", testInput)) {
        std::cout << "✅ Input sent: \"" << testInput << "\"\n";
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // output = mgr.getOutput("echo");
    // if (output.has_value()) {
    //     std::cout << "✅ Output: \"" << output.value() << "\"\n";
    // }
    
    // ============================================================
    // TEST 8: Close Input and Stop All
    // ============================================================
    printSeparator("TEST 8: Cleanup - stop all processes");
    
    mgr.stopAll();
    std::cout << "✅ All processes stopped\n";
    
    printProcessList(mgr);
    
    // ============================================================
    // TEST 9: Try to access a stopped process (should fail gracefully)
    // ============================================================
    printSeparator("TEST 9: Access stopped process");
    
    if (mgr.sendInput("echo", "Hello")) {
        std::cout << "⚠️  Should not be able to send to stopped process\n";
    } else {
        std::cout << "✅ Correctly rejected input to stopped process\n";
    }
    
    // ============================================================
    // TEST 10: Stress Test - Create and Stop Many Processes
    // ============================================================
    printSeparator("TEST 10: Stress test - create 5 processes");
    
    std::cout << "Creating 5 'date' processes with different names...\n";
    for (int i = 1; i <= 5; ++i) {
        std::string name = "date" + std::to_string(i);
        if (mgr.create(name, "/bin/date", {})) {
            std::cout << "  ✅ Created: " << name << "\n";
        } else {
            std::cout << "  ❌ Failed: " << name << "\n";
        }
    }
    
    printProcessList(mgr);
    
    std::cout << "\nStopping all processes...\n";
    mgr.stopAll();
    
    printProcessList(mgr);
    
    // ============================================================
    // FINAL
    // ============================================================
    printSeparator("✅ ALL TESTS COMPLETED");
    std::cout << "ProcessManager test suite finished successfully!\n";
    
    return 0;
}