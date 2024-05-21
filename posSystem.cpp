

#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <iomanip>

class IItem {
public:
    virtual std::string GetName() const = 0;
    virtual float GetPrice() const = 0;
    virtual ~IItem() = default; // Ensure proper cleanup of derived classes
};

class Item : public IItem {
private:
    std::string name;
    float price;

public:
    Item(std::string n, float p) : name(std::move(n)), price(p) {}

    std::string GetName() const override { return name; }
    float GetPrice() const override { return price; }
};

class DiscountedItem : public Item {
private:
    float discount;

public:
    DiscountedItem(std::string n, float p, float d) : Item(std::move(n), p), discount(d) {}

    float GetPrice() const override { return Item::GetPrice() - (Item::GetPrice() * discount / 100); }
};

class OnSaleItem : public Item {
private:
    bool onSale;

public:
    OnSaleItem(std::string n, float p, bool sale) : Item(std::move(n), p), onSale(sale) {}

    bool IsOnSale() const { return onSale; }
};

class Menu {
private:
    std::unordered_map<std::string, float> items{
            {"Rice", 13.00f},
            {"Cow", 14.00f},
            {"Money", 15.00f},
            {"Chicken", 32.8f},
            {"RedBull", 2.99f},
            {"Monster", 2.99f},
            {"Piza", 10.99f},
            // Add more items here
    };

public:
    void DisplayMenu() const {
        std::cout << std::string(50, '-') << std::endl;
        std::cout << "Welcome to the POS System\n\n";
        std::cout << std::setw(20) << std::left << "Item" << std::setw(30) << std::right << "Price" << std::endl;
        std::cout << std::string(50, '-') << std::endl;

        for (const auto& item : items) {
            std::cout << std::setw(20) << std::left << item.first << std::setw(30) << std::right << std::fixed << std::setprecision(2) << item.second << std::endl;
        }

        std::cout << std::string(50, '-') << std::endl;
    }

    const IItem* GetItem(const std::string& name) const {
        auto it = items.find(name);
        if (it != items.end()) {
            return new Item(it->first, it->second);
        } else {
            std::cout << "Item not found" << std::endl;
            return nullptr;
        }
    }
};

class Cart {
private:
    std::unordered_map<std::string, const IItem*> cart;

public:
    ~Cart() {
        for (auto& item : cart) {
            delete item.second;
        }
    }

    void AddToCart(const std::string& name, float price) {
        cart[name] = new Item(name, price);
        std::cout << name << " added" << std::endl;
        std::cout << GetItemNames() << std::endl;
    }

    void RemoveFromCart(const std::string& name) {
        auto it = cart.find(name);
        if (it != cart.end()) {
            delete it->second;
            cart.erase(it);
        }
    }

    void ClearCart() {
        for (auto& item : cart) {
            delete item.second;
        }
        cart.clear();
    }

    std::string GetItemNames() const {
        if (cart.empty()) {
            return "The cart is empty";
        }
        std::string result;
        for (const auto& item : cart) {
            result += item.first + ", ";
        }
        return result;
    }

    float GetTotal() const {
        float total = 0.0f;
        for (const auto& item : cart) {
            total += item.second->GetPrice();
        }
        return total;
    }

    void GetCartStat() const {
        std::cout << "Item\t:\tPrice\n\n";
        if (cart.empty()) {
            std::cout << "The cart is empty" << std::endl;
        } else {
            for (const auto& item : cart) {
                std::cout << item.first << "\t: " << item.second->GetPrice() << std::endl;
            }
            std::cout << "Total amount : " << GetTotal() << std::endl;
        }
    }
};

int main() {
    Menu menu;
    Cart cart;

    menu.DisplayMenu(); // Display the menu at the top

    std::cout << "It is a command based self checkout section." << std::endl
              << "You can either select any item from the given menu or put your custom item along with the price." << std::endl
              << "At the end, it will ask you to put the payment method, you can then put in the cash amount and then it will return your change." << std::endl;

    const std::string commands = "To Add item in Your Cart , type ADD\n"
                                 "To Remove an Item from your Cart, type DEL\n"
                                 "To Proceed to Checkout, type CHK\n"
                                 "To check the cart stat, type STAT\n"
                                 "To quit in the middle, leave the items and type QUIT\n";

    while (true) {
        std::cout << commands << std::endl;
        std::string value;
        std::cout << "> ";
        std::cin >> value;

        if (value == "ADD") {
            std::string choice;
            std::cout << "Do you want something from Menu or Custom (M/C)? ";
            std::cin >> choice;
            if (choice == "M") {
                std::string itemName;
                do {
                    std::cout << "What item would you like? ";
                    std::cin >> itemName;
                } while (menu.GetItem(itemName) == nullptr);
                const IItem* selectedItem = menu.GetItem(itemName);
                cart.AddToCart(selectedItem->GetName(), selectedItem->GetPrice());
            } else {
                std::string name;
                std::cout << "Enter the name of Item: ";
                std::cin >> name;
                float price;
                std::cout << "Enter the price: ";
                std::cin >> price;
                cart.AddToCart(name, price);
            }
        } else if (value == "DEL") {
            std::string itemName;
            std::cout << "Enter the name of Item: ";
            std::cin >> itemName;
            cart.RemoveFromCart(itemName);
        } else if (value == "CHK") {
            std::cout << cart.GetTotal() << std::endl;
            float input;
            std::cout << "Enter the payment amount: ";
            std::cin >> input;
            if (input >= cart.GetTotal()) { // Changed condition to allow equal or greater
                std::cout << "Processing payment ";
                for (int i = 0; i < 5; i++) {
                    std::cout << ".";
                     std::this_thread::sleep_for(std::chrono::milliseconds(500));
                }
                std::cout << std::endl;
                float change = input - cart.GetTotal();
                std::cout << "Change: " << change << std::endl;
                cart.ClearCart(); // Clear the cart after checkout
            } else {
                std::cout << "You do not have enough money" << std::endl;
            }
        } else if (value == "STAT") {
            cart.GetCartStat();
        } else if (value == "QUIT") {
            std::cout << "SEE You AGAIN! " << std::endl;
            break;
        }
    }

    return 0;
}

