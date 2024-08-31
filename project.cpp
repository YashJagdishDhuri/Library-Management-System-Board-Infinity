#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <ctime>
#include <algorithm>

using namespace std;

const string DATA_FILE = "library_data.txt";
const int FINE_PER_DAY = 50;

class Book {
public:
    int id;
    string title;
    string author;
    bool isIssued;
    time_t dueDate;
    int issueCount;

    Book(int id, string title, string author)
        : id(id), title(title), author(author), isIssued(false), dueDate(0), issueCount(0) {}

    void display() {
        cout << "ID: " << id << "\nTitle: " << title << "\nAuthor: " << author
             << "\nStatus: " << (isIssued ? "Issued" : "Available")
             << "\nDue Date: " << (isIssued ? ctime(&dueDate) : "N/A")
             << "\nIssue Count: " << issueCount << endl;
    }
};

class Library {
private:
    vector<Book> books;

    int findBookIndexById(int id) {
        for (int i = 0; i < books.size(); ++i) {
            if (books[i].id == id) return i;
        }
        return -1;
    }

    void saveToFile() {
        ofstream file(DATA_FILE, ios::out | ios::trunc);
        for (const auto& book : books) {
            file << book.id << "\n" << book.title << "\n" << book.author << "\n"
                 << book.isIssued << "\n" << book.dueDate << "\n" << book.issueCount << "\n";
        }
        file.close();
    }

    void loadFromFile() {
        ifstream file(DATA_FILE);
        if (!file.is_open()) return;

        while (!file.eof()) {
            int id;
            string title, author;
            bool isIssued;
            time_t dueDate;
            int issueCount;

            file >> id;
            file.ignore();
            getline(file, title);
            getline(file, author);
            file >> isIssued;
            file >> dueDate;
            file >> issueCount;

            Book book(id, title, author);
            book.isIssued = isIssued;
            book.dueDate = dueDate;
            book.issueCount = issueCount;

            books.push_back(book);
        }
        file.close();
    }

public:
    Library() {
        loadFromFile();
    }

    ~Library() {
        saveToFile();
    }

    void addBook(int id, string title, string author) {
        if (findBookIndexById(id) != -1) {
            cout << "\033[31mBook with ID " << id << " already exists.\033[0m\n";
            return;
        }
        books.push_back(Book(id, title, author));
        cout << "\033[32mBook added successfully.\033[0m\n";
        saveToFile();
    }

    void searchBook(int id) {
        int index = findBookIndexById(id);
        if (index != -1) {
            books[index].display();
        } else {
            cout << "\033[31mBook not found.\033[0m\n";
        }
    }

    void issueBook(int id) {
        listAvailableBooks();
        int index = findBookIndexById(id);
        if (index != -1 && !books[index].isIssued) {
            books[index].isIssued = true;
            books[index].dueDate = time(nullptr) + 7 * 24 * 60 * 60; // Set due date to 7 days from now
            books[index].issueCount++;
            cout << "\033[32mBook issued successfully.\033[0m\n";
            cout << "Due Date: " << ctime(&books[index].dueDate) << endl;
        } else {
            cout << (index == -1 ? "\033[31mBook not found.\033[0m\n" : "\033[31mBook already issued.\033[0m\n");
        }
        saveToFile();
    }

    void returnBook(int id) {
        int index = findBookIndexById(id);
        if (index != -1 && books[index].isIssued) {
            books[index].isIssued = false;
            time_t currentTime = time(nullptr);
            if (currentTime > books[index].dueDate) {
                int daysLate = (currentTime - books[index].dueDate) / (24 * 60 * 60);
                int fine = daysLate * FINE_PER_DAY;
                cout << "\033[31mBook returned late. Fine: Rs. " << fine << ".\033[0m\n";
            } else {
                cout << "\033[32mBook returned on time.\033[0m\n";
            }
        } else {
            cout << (index == -1 ? "\033[31mBook not found.\033[0m\n" : "\033[31mBook was not issued.\033[0m\n");
        }
        saveToFile();
    }

    void listBooks() {
        if (books.empty()) {
            cout << "\033[31mNo books available in the library.\033[0m\n";
            return;
        }
        for (auto& book : books) {
            book.display();
            cout << "---------------------\n";
        }
    }

    void deleteBook(int id) {
        int index = findBookIndexById(id);
        if (index != -1) {
            if (books[index].isIssued) {
                cout << "\033[31mCannot delete an issued book.\033[0m\n";
            } else {
                books.erase(books.begin() + index);
                cout << "\033[32mBook deleted successfully.\033[0m\n";
            }
        } else {
            cout << "\033[31mBook not found.\033[0m\n";
        }
        saveToFile();
    }

    void recommendBooks() {
        if (books.empty()) {
            cout << "\033[31mNo books available currently in top borrowed books.\033[0m\n";
            return;
        }

        sort(books.begin(), books.end(), [](const Book& a, const Book& b) {
            return a.issueCount > b.issueCount;
        });

        cout << "\033[34mTop Borrowed Books:\033[0m\n";
        for (int i = 0; i < min((int)books.size(), 5); ++i) {
            cout << books[i].title << " (Issued " << books[i].issueCount << " times)\n";
        }
    }

    void listAvailableBooks() {
        if (books.empty()) {
            cout << "\033[31mNo books available in the library.\033[0m\n";
            return;
        }

        cout << "\033[34mAvailable Books:\033[0m\n";
        for (const auto& book : books) {
            if (!book.isIssued) {
                cout << "ID: " << book.id << ", Title: " << book.title << ", Author: " << book.author << endl;
            }
        }
    }
};

void displayMenu() {
    cout << "\033[36m========================================\033[0m\n";
    cout << "\033[33m     WELCOME TO THE LIBRARY SYSTEM      \033[0m\n";
    cout << "\033[36m========================================\033[0m\n";
    cout << "1. Add New Book\n";
    cout << "2. Search for a Book\n";
    cout << "3. Issue a Book\n";
    cout << "4. Return a Book\n";
    cout << "5. List All Books\n";
    cout << "6. Delete a Book\n";
    cout << "7. Recommend Books\n";
    cout << "8. Exit\n";
    cout << "\033[36m========================================\033[0m\n";
    cout << "Enter your choice: ";
}

int main() {
    Library library;
    int choice, id;
    string title, author;

    while (true) {
        displayMenu();
        cin >> choice;

        switch (choice) {
            case 1:
                cout << "Enter Book ID: ";
                cin >> id;
                cin.ignore();
                cout << "Enter Book Title: ";
                getline(cin, title);
                cout << "Enter Book Author: ";
                getline(cin, author);
                library.addBook(id, title, author);
                break;
            case 2:
                cout << "Enter Book ID: ";
                cin >> id;
                library.searchBook(id);
                break;
            case 3:
                library.listAvailableBooks();
                cout << "Enter Book ID: ";
                cin >> id;
                library.issueBook(id);
                break;
            case 4:
                cout << "Enter Book ID: ";
                cin >> id;
                library.returnBook(id);
                break;
            case 5:
                library.listBooks();
                break;
            case 6:
                cout << "Enter Book ID: ";
                cin >> id;
                library.deleteBook(id);
                break;
            case 7:
                library.recommendBooks();
                break;
            case 8:
                cout << "Exiting...\n";
                return 0;
            default:
                cout << "\033[31mInvalid choice. Please try again.\033[0m\n";
        }
    }
}
