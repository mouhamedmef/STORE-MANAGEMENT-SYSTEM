#include <iostream>
#include <string>
#include <list>
#include <deque>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <sstream>

using namespace std;

class Product {
private:
    string name;
    string barcode;
    double price;
    int discount;
    int stock;

public:
    Product(string name = "", string barcode = "", double price = 0.0,
            int discount = 0, int stock = 0)
        : name(name), barcode(barcode), price(price),
          discount(discount), stock(stock) {}

    string getName() const { return name; }
    string getBarcode() const { return barcode; }
    double getPrice() const { return price; }
    int getDiscount() const { return discount; }
    int getStock() const { return stock; }

    void setName(string n) { name = n; }
    void setBarcode(string b) { barcode = b; }
    void setPrice(double p) { price = p; }
    void setDiscount(int d) { discount = d; }
    void setStock(int s) { stock = s; }

    void decrementStock() { if (stock > 0) stock--; }
    void incrementStock() { stock++; }

    double getFinalPrice() const {
        return price * (1 - discount / 100.0);
    }
};

class Cart {
private:
    list<Product> products;
    double total;

public:
    Cart() : total(0.0) {}

    void addProduct(const Product& p) {
        products.push_back(p);
        total += p.getFinalPrice();
    }

    bool removeProduct(const string& barcode) {
        for (auto it = products.begin(); it != products.end(); ++it) {
            if (it->getBarcode() == barcode) {
                total -= it->getFinalPrice();
                products.erase(it);
                return true;
            }
        }
        return false;
    }

    double getTotal() const { return total; }
    const list<Product>& getProducts() const { return products; }
    void clear() { products.clear(); total = 0.0; }
};

class Cashier {
private:
    string name;
    int pin;

public:
    Cashier(string name = "", int pin = 0) : name(name), pin(pin) {}

    bool verifyPin(int inputPin) const {
        return pin == inputPin;
    }

    string getName() const { return name; }
};

class Admin {
private:
    string email;
    string password;
    static deque<Product> inventory;

public:
    Admin(string email = "", string password = "")
        : email(email), password(password) {
        loadFromFile(); // Load inventory from file when Admin is created
    }

    bool verifyCredentials(const string& inputEmail, const string& inputPassword) const {
        return email == inputEmail && password == inputPassword;
    }

    void addProduct(const Product& p) {
        inventory.push_back(p);
        saveToFile();
    }

    void removeProduct(const string& barcode) {
        for (auto it = inventory.begin(); it != inventory.end(); ++it) {
            if (it->getBarcode() == barcode) {
                inventory.erase(it);
                break;
            }
        }
        saveToFile();
    }

    void modifyProduct(const string& barcode, const Product& newProduct) {
        for (auto& product : inventory) {
            if (product.getBarcode() == barcode) {
                product = newProduct;
                break;
            }
        }
        saveToFile();
    }

    static Product* findProduct(const string& barcode) {
        for (auto& product : inventory) {
            if (product.getBarcode() == barcode) {
                return &product;
            }
        }
        return nullptr;
    }

    void displayInventory() const {
        cout << "\n============= Current Inventory =============\n";
        cout << setw(30) << left << "Name"
             << setw(15) << right << "Price"
             << setw(10) << "Discount"
             << setw(10) << "Stock" << endl;
        cout << string(70, '-') << endl;

        for (const auto& product : inventory) {
            cout << setw(30) << left << product.getName()
                 << setw(15) << right << fixed << setprecision(2) << product.getPrice()
                 << setw(10) << product.getDiscount() << "%"
                 << setw(10) << product.getStock() << endl;
        }
        cout << string(70, '-') << endl;
    }

private:
    void saveToFile() const {
        ofstream file("inventory.txt");
        if (file.is_open()) {
            for (const auto& product : inventory) {
                file << product.getName() << ","
                     << product.getBarcode() << ","
                     << product.getPrice() << ","
                     << product.getDiscount() << ","
                     << product.getStock() << endl;
            }
            file.close();
        } else {
            cerr << "Error: Unable to save inventory to file!" << endl;
        }
    }

    void loadFromFile() {
        ifstream file("inventory.txt");
        if (file.is_open()) {
            inventory.clear();
            string line;
            while (getline(file, line)) {
                stringstream ss(line);
                string name, barcode;
                double price;
                int discount, stock;
                char comma;

                getline(ss, name, ',');
                getline(ss, barcode, ',');
                ss >> price >> comma >> discount >> comma >> stock;
                inventory.push_back(Product(name, barcode, price, discount, stock));
            }
            file.close();
        } else {
            cerr << "Error: Unable to load inventory from file!" << endl;
        }
    }
};

// Initialize static inventory
deque<Product> Admin::inventory;

class Invoice {
private:
    Cart cart;
    Cashier cashier;
    time_t timestamp;

public:
    Invoice(const Cart& cart, const Cashier& cashier)
        : cart(cart), cashier(cashier) {
        timestamp = time(nullptr);
    }

    void print() const {
        cout << "\n=================== INVOICE ===================" << endl;
        cout << "Date: " << ctime(&timestamp);
        cout << "Cashier: " << cashier.getName() << endl;
        cout << "\nProducts:" << endl;
        cout << setw(30) << left << "Name"
             << setw(15) << right << "Price"
             << setw(10) << "Discount"
             << setw(15) << "Final Price" << endl;
        cout << string(70, '-') << endl;

        for (const auto& product : cart.getProducts()) {
            cout << setw(30) << left << product.getName()
                 << setw(15) << right << fixed << setprecision(2) << product.getPrice()
                 << setw(10) << product.getDiscount() << "%"
                 << setw(15) << product.getFinalPrice() << endl;
        }

        cout << string(70, '-') << endl;
        cout << "Total: $" << fixed << setprecision(2) << cart.getTotal() << endl;
        cout << "=============================================" << endl;
    }
};

class Interface {
private:
    Admin admin;

    void showAdminMenu() {
        while (true) {
            cout << "\n\t\t\t Administrator Menu\n";
            cout << "\t\t1) Add new product\n";
            cout << "\t\t2) Modify product\n";
            cout << "\t\t3) Remove product\n";
            cout << "\t\t4) View inventory\n";
            cout << "\t\t5) Back to Main menu\n";
            cout << "\n\t\tPlease Enter your choice: ";

            int choice;
            cin >> choice;

            switch(choice) {
                case 1: addProduct(); break;
                case 2: modifyProduct(); break;
                case 3: removeProduct(); break;
                case 4: admin.displayInventory(); break;
                case 5: return;
                default: cout << "\t\tInvalid Choice!" << endl;
            }
        }
    }

    void addProduct() {
        string name, barcode;
        double price;
        int discount, stock;

        cout << "\nEnter product details:\n";
        cout << "Name: ";
        cin.ignore();
        getline(cin, name);
        cout << "Barcode: ";
        cin >> barcode;
        cout << "Price: ";
        cin >> price;
        cout << "Discount (%): ";
        cin >> discount;
        cout << "Initial stock: ";
        cin >> stock;

        admin.addProduct(Product(name, barcode, price, discount, stock));
        cout << "\nProduct added successfully!" << endl;
    }

    void modifyProduct() {
        string barcode, name;
        double price;
        int discount, stock;

        admin.displayInventory();
        cout << "\nEnter product barcode to modify: ";
        cin >> barcode;

        cout << "\nEnter new product details:\n";
        cout << "Name: ";
        cin.ignore();
        getline(cin, name);
        cout << "Price: ";
        cin >> price;
        cout << "Discount (%): ";
        cin >> discount;
        cout << "Stock: ";
        cin >> stock;

        admin.modifyProduct(barcode, Product(name, barcode, price, discount, stock));
        cout << "\nProduct modified successfully!" << endl;
    }

    void removeProduct() {
        string barcode;
        admin.displayInventory();
        cout << "\nEnter product barcode to remove: ";
        cin >> barcode;
        admin.removeProduct(barcode);
        cout << "\nProduct removed successfully!" << endl;
    }

    void addToCart(Cart& cart) {
        string barcode;
        admin.displayInventory();

        cout << "\nEnter product barcode: ";
        cin >> barcode;

        Product* product = Admin::findProduct(barcode);
        if (product != nullptr) {
            if (product->getStock() > 0) {
                cart.addProduct(*product);
                product->decrementStock();
                cout << "\nProduct added to cart successfully!" << endl;
            } else {
                cout << "\nProduct out of stock!" << endl;
            }
        } else {
            cout << "\nProduct not found in inventory!" << endl;
        }
    }

    void removeFromCart(Cart& cart) {
        string barcode;
        cout << "\nEnter product barcode to remove: ";
        cin >> barcode;

        if (cart.removeProduct(barcode)) {
            Product* product = Admin::findProduct(barcode);
            if (product != nullptr) {
                product->incrementStock();
                cout << "\nProduct removed from cart successfully!" << endl;
            }
        } else {
            cout << "\nProduct not found in cart!" << endl;
        }
    }

    void viewCart(const Cart& cart) {
        cout << "\n============= Current Cart =============\n";
        cout << setw(30) << left << "Name"
             << setw(15) << right << "Price"
             << setw(10) << "Discount"
             << setw(15) << "Final Price" << endl;
        cout << string(70, '-') << endl;

        for (const auto& product : cart.getProducts()) {
            cout << setw(30) << left << product.getName()
                 << setw(15) << right << fixed << setprecision(2) << product.getPrice()
                 << setw(10) << product.getDiscount() << "%"
                 << setw(15) << product.getFinalPrice() << endl;
        }

        cout << string(70, '-') << endl;
        cout << "Total: $" << fixed << setprecision(2) << cart.getTotal() << endl;
    }

    void generateInvoice(const Cart& cart, const Cashier& cashier) {
        if (cart.getProducts().empty()) {
            cout << "\nCart is empty! Cannot generate invoice." << endl;
            return;
        }
        Invoice invoice(cart, cashier);
        invoice.print();
        cout << "\nInvoice generated successfully!" << endl;
    }

    void showCashierMenu(const Cashier& cashier) {
        Cart currentCart;

        while (true) {
            cout << "\n\t\t\t Cashier Menu\n";
            cout << "\t\t1) Add product to cart\n";
            cout << "\t\t2) Remove product from cart\n";
            cout << "\t\t3) View current cart\n";
            cout << "\t\t4) View inventory\n";
            cout << "\t\t5) Generate invoice\n";
            cout << "\t\t6) Back to Main menu\n";
            cout << "\n\t\tPlease Enter your choice: ";

            int choice;
            cin >> choice;

            switch(choice) {
                case 1: addToCart(currentCart); break;
                case 2: removeFromCart(currentCart); break;
                case 3: viewCart(currentCart); break;
                case 4: admin.displayInventory(); break;
                case 5:
                    generateInvoice(currentCart, cashier);
                    currentCart.clear(); // Clear the cart after generating invoice
                    break;
                case 6: return;
                default: cout << "\t\tInvalid Choice!" << endl;
            }
        }
    }

public:
    Interface() : admin("admin@store.com", "admin123") {}

    void start() {
        while (true) {
            cout << "\n\t\t\t Store Management System\n";
            cout << "\t\t1) Admin Login\n";
            cout << "\t\t2) Cashier Login\n";
            cout << "\t\t3) Exit\n";
            cout << "\n\t\tPlease Enter your choice: ";

            int choice;
            cin >> choice;

            switch(choice) {
                case 1: handleAdminLogin(); break;
                case 2: handleCashierLogin(); break;
                case 3:
                    cout << "\nThank you for using our system!\n";
                    return;
                default:
                    cout << "\t\tInvalid Choice!" << endl;
            }
        }
    }

private:
    void handleAdminLogin() {
        string email, password;
        cout << "\nEnter email: ";
        cin >> email;
        cout << "Enter password: ";
        cin >> password;

        if (admin.verifyCredentials(email, password)) {
            showAdminMenu();
        } else {
            cout << "\nInvalid credentials!" << endl;
        }
    }

    void handleCashierLogin() {
        string name;
        int pin;

        cout << "\nEnter name: ";
        cin.ignore();
        getline(cin, name);
        cout << "Enter PIN (4 digits): ";
        cin >> pin;

        Cashier cashier("demo_cashier", 1234);

        if (cashier.verifyPin(pin)) {
            showCashierMenu(cashier);
        } else {
            cout << "\nInvalid credentials!" << endl;
        }
    }
};

int main() {
    Interface interface;
    interface.start();
    return 0;
}
