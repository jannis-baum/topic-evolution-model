#include <exception>
#include <string>

using namespace std;

class TEException: public exception {
private:
    string message;

public:
    TEException(const char* msg): message(msg) {}

    const char* what() const throw() {
        return message.c_str();
    }
};
