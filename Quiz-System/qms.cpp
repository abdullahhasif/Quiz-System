// Header Files
#include <sstream>
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib> 
#include <iomanip>  // for setw and setfill

using namespace std;

// Base class for User (Teacher and Student)

class Quiz;

// Hierarchial Inheritance
class User {
protected:
    string username;
    string password;
    string designation;
    string courseID;

public:

    User() {}

    User(const string& username, const string& password, const string& designation, const string& courseID)
        : username(username), password(password), designation(designation), courseID(courseID) {}

    //virtual bool login(const string& username, const string& password);

    string getUsername() const { return username; }
    string getPassword() const { return password; }
    string getDesignation() const { return designation; }
    string getCourseID() const { return courseID; }
};

// Teacher class inherits from User
class Teacher : public User {
public:
    Teacher(const string& username, const string& password, const string& designation, const string& courseID)
        : User(username, password, designation, courseID) {}

    void createQuiz(const string& name, const int numQuestions);
    void modifyQuiz(const string& quizName);
    void displayQuizzes() const;
};

// Student class inherits from User
class Student : public User {
public:
    Student(const string& username, const string& password, const string& designation, const string& courseID)
        : User(username, password, designation, courseID) {}

    void takeQuiz(const Quiz& quiz, const User& user);
    double getGrade(const Quiz& quiz);
};

// Represents a single question
class Question {
public:
    string questionText;
    string options[4]; // Fixed size for 4 options (can be adjusted based on requirements)
    int correctAnswerIndex;


    // Default constructor (no arguments)
    Question() : questionText(""), correctAnswerIndex(-1) {
        for (int i = 0; i < 4; ++i) {
            options[i] = ""; // Initialize options to empty strings
        }
    }

    Question(const string& questionText, const string options[4], int correctAnswerIndex)
        : questionText(questionText), correctAnswerIndex(correctAnswerIndex) {
        for (int i = 0; i < 4; ++i) {
            this->options[i] = options[i];
        }
    }
};

// Represents a quiz
class Quiz {
public:
    string name;
    int numQuestions;

    // Aggregation
    Question* questions; // Array of questions (dynamically allocated)

    Quiz(const string& name, const int numQuestions)
        : name(name), numQuestions(numQuestions) {
        questions = new Question[numQuestions];
    }

    ~Quiz() {
        delete[] questions; // Deallocate memory in destructor
    }

    double calculateGrade(const int answers[]) const;

    void displayQuiz() const {
        cout << "Quiz Name: " << name << endl;
        for (int i = 0; i < numQuestions; ++i) {
            cout << "Question " << (i + 1) << ": " << questions[i].questionText << endl;
            for (int j = 0; j < 4; ++j) {
                cout << "- Option " << (j + 1) << ": " << questions[i].options[j] << endl;
            }
        }
    }
};

// Function prototypes for file I/O operations
bool writeUserData(const User& user, const string& filename);
User readUserData(const string& filename, const string& username, const string& password);
bool writeQuizData(const Quiz& quiz, const Question* questions, const string& courseID);
Quiz readQuizData(const string& filename, const string& courseID, const string& quizName);
bool quizExists(const string& courseID, const string& quizName);
void showQuiz(const string& courseID, const string& quizName);
void displayQuiz(const Quiz& quiz); // Separate function to display a Quiz

// Implementation of writeUserData function (stores each user in a separate line)
bool writeUserData(const User& user, const string& filename) {
    ofstream outfile(filename.c_str(), ios::app); // Open in append mode

    string hash = user.getPassword();

    for (int i = 0; i < hash.length(); ++i) {
        hash[i] = hash[i] + 3;
    }

    if (outfile.is_open()) {
        outfile << user.getUsername() << "," << hash << "," << user.getDesignation() << "," << user.getCourseID() << endl;
        outfile.close();
        return true;
    } else {
        //cerr << "Error: Could not open file " << filename << endl;
        return false;
    }
}

User readUserData(const string& filename, const string& username, const string& password) {
    ifstream infile(filename.c_str());
    if (!infile.is_open()) {
        cerr << "Error: Could not open file " << filename << endl;
        return User("", "", "", ""); // Return empty User object on error
    }

    string line;
    while (getline(infile, line)) {
        istringstream iss(line);
        string readUsername, readPassword, readdesignation, readCourseID;
        getline(iss, readUsername, ',');
        getline(iss, readPassword, ',');
        getline(iss, readdesignation, ',');
        getline(iss, readCourseID, ',');

        string unhashed = readPassword;

        for (int i = 0; i < unhashed.length(); ++i) {
            unhashed[i] = unhashed[i] - 3;
        }

        if (username == readUsername && password == unhashed) {
            infile.close();
            return User(string(readUsername.c_str()), string(unhashed.c_str()),
                        string(readdesignation.c_str()), string(readCourseID.c_str()));
        }
    }

    infile.close();
    return User("", "", "", ""); // Return empty User object if user not found
}

// Implementation of writeQuizData function 
bool writeQuizData(const Quiz& quiz, const Question* questions, const string& courseID) {
    string quizFilename = courseID + "_" + quiz.name + ".txt";
    ofstream outfile(quizFilename.c_str()); // Open in write mode (overwrites existing content)
    if (outfile.is_open()) {
        outfile << quiz.numQuestions << endl;  // Write number of questions first
        for (int i = 0; i < quiz.numQuestions; ++i) {
            outfile << questions[i].questionText << endl;
            for (int j = 0; j < 4; ++j) {
                outfile << questions[i].options[j] << endl;
            }
            outfile << questions[i].correctAnswerIndex << endl;
        }
        outfile.close();

        string storequizzes = courseID + ".txt";
        ifstream infile(storequizzes.c_str());
        stringstream content;
        string line;
        while (getline(infile, line)) {
            if (line != quiz.name) {
                content << line << endl;
            }
        }
        infile.close();

        ofstream writefile(storequizzes.c_str());
        if (writefile.is_open()) {
            writefile << content.str();
            writefile << quiz.name << endl; // Add the quiz name back
        }
        return true;
    } else {
        //cerr << "Error: Could not open file " << quizFilename << endl;
        return false;
    }
}

// Implementation of readQuizData function
Quiz readQuizData(const string& courseID, const string& quizName) {
    string quizFilename = courseID + "_" + quizName + ".txt";
    ifstream infile(quizFilename.c_str());
    if (!infile.is_open()) {
        cerr << "Error: Could not open file " << quizFilename << endl;
        return Quiz("", 0); // Return empty Quiz object on error
    }

    int numQuestions;
    infile >> numQuestions;
    infile.ignore(); // Consume newline character after numQuestions

    Quiz quiz(quizName, numQuestions);
    for (int i = 0; i < numQuestions; ++i) {
        getline(infile, quiz.questions[i].questionText);
        for (int j = 0; j < 4; ++j) {
            getline(infile, quiz.questions[i].options[j]);
        }
        infile >> quiz.questions[i].correctAnswerIndex;
        infile.ignore(); // Consume newline character after correctAnswerIndex
    }

    infile.close();
    return quiz;
}

// Function to check if a quiz exists for a given course
bool quizExists(const string& courseID, const string& quizName) {
    string quizzesFilename = courseID + ".txt";
    ifstream infile(quizzesFilename.c_str());
    if (!infile.is_open()) {
        cerr << "Error: Could not open file " << quizzesFilename << endl;
        return false;
    }

    string line;
    while (getline(infile, line)) {
        if (line == quizName) {
            infile.close();
            return true; // Quiz exists
        }
    }

    infile.close();
    return false; // Quiz doesn't exist
}

// Function to show a quiz
void showQuiz(const string& courseID, const string& quizName) {
    Quiz quiz = readQuizData(courseID, quizName);
    if (quiz.name.empty()) {
        cerr << "Error: Could not find quiz " << quizName << endl;
    } else {
        displayQuiz(quiz); // Display the quiz
    }
}

// Separate function to display a Quiz
void displayQuiz(const Quiz& quiz) {
    cout << "Quiz Name: " << quiz.name << endl;
    for (int i = 0; i < quiz.numQuestions; ++i) {
        cout << "Question " << (i + 1) << ": " << quiz.questions[i].questionText << endl;
        for (int j = 0; j < 4; ++j) {
            cout << "- Option " << (j + 1) << ": " << quiz.questions[i].options[j] << endl;
        }
    }
}


double Quiz::calculateGrade(const int answers[]) const {
    int correctAnswers = 0;
    for (int i = 0; i < numQuestions; ++i) {
        if (answers[i] == questions[i].correctAnswerIndex) {
            correctAnswers++;
        }
    }
    return static_cast<double>(correctAnswers) / numQuestions * 100.0; // Calculate percentage
}

void Student::takeQuiz(const Quiz& quiz, const User& user) {
    // Display the quiz to the student
    quiz.displayQuiz();

    // Prepare variables to store student's answers
    int answers[quiz.numQuestions];

    // Get student's answer for each question
    for (int i = 0; i < quiz.numQuestions; ++i) {
        int answer;
        do {
            cout << "Enter your answer for question " << (i + 1) << " (1-4): ";
            cin >> answer;
        } while (answer < 1 || answer > 4);
        answers[i] = answer - 1; // Adjust for zero-based indexing
    }

    // Calculate the student's grade (call calculateGrade from Quiz)
    double grade = quiz.calculateGrade(answers);

    // Display the grade
    cout << "Your grade for " << quiz.name << " is: " << grade << "%" << endl;

    // Storing Quizzes attempted
    string writeQuiz = user.getCourseID() + "_" + user.getUsername() + ".txt";

    ofstream writefile(writeQuiz.c_str(), ios::app);

    if (writefile.is_open()) {
        writefile << quiz.name << "," << grade << "%" << endl;
    }
}

void Teacher::createQuiz(const string& name, const int numQuestions) {
    // Input validation 
    if (numQuestions <= 0) {
        cerr << "Error: Invalid number of questions. Please enter a positive value." << endl;
        return;
    }

    // Allocate memory for questions
    Question* questions = new Question[numQuestions];

    // Prompt teacher for each question, options, and correct answer
    for (int i = 0; i < numQuestions; ++i) {
        cin.ignore();
        cout << "\nEnter question " << i + 1 << ":" << endl;
        getline(cin, questions[i].questionText); // Capture question statement

        for (int j = 0; j < 4; ++j) {
            cout << "Enter option " << (j + 1) << ": ";
            getline(cin, questions[i].options[j]);
        }

        int correctAnswer;
        do {
            cout << "Enter the correct answer index (1-4): ";
            cin >> correctAnswer;
        } while (correctAnswer < 1 || correctAnswer > 4);
        questions[i].correctAnswerIndex = correctAnswer - 1; // Adjust for zero-based indexing
    }

    // Write quiz data to file
    if (!writeQuizData(Quiz(name, numQuestions), questions, this->courseID)) {
        // Handle error if writing to file fails (optional)
        cerr << "Error: Could not write quiz data to file." << endl;
    }

    // Deallocate memory
    delete[] questions;
    cout << "Quiz " << name << " created successfully!" << endl;
}

void Teacher::modifyQuiz(const string& quizName) {
    string quizFilename = this->courseID + "_" + quizName + ".txt";
    ifstream infile(quizFilename.c_str());
    if (!infile.is_open()) {
        cerr << "Error: Could not open file " << quizFilename << endl;
        return;
    }

    int numQuestions;
    infile >> numQuestions;
    infile.ignore(); // Consume newline character

    Quiz quiz(quizName, numQuestions);

    // Read existing questions from file
    for (int i = 0; i < numQuestions; ++i) {
        getline(infile, quiz.questions[i].questionText);
        for (int j = 0; j < 4; ++j) {
            getline(infile, quiz.questions[i].options[j]);
        }
        infile >> quiz.questions[i].correctAnswerIndex;
        infile.ignore(); // Consume newline character
    }

    infile.close();

    // Display current quiz to the teacher for review
    cout << "\nCurrent Quiz Data:\n";
    displayQuiz(quiz);

    // Ask the teacher which question they want to modify
    int questionIndex;
    do {
        cout << "\nEnter the question number you want to modify (1-" << numQuestions << "): ";
        cin >> questionIndex;
    } while (questionIndex < 1 || questionIndex > numQuestions);
    questionIndex--; // Adjust for zero-based indexing

    // Prompt teacher for new question details
    cin.ignore();
    cout << "\nEnter the new question text: ";
    getline(cin, quiz.questions[questionIndex].questionText);

    for (int j = 0; j < 4; ++j) {
        cout << "Enter option " << (j + 1) << ": ";
        getline(cin, quiz.questions[questionIndex].options[j]);
    }

    int correctAnswer;
    do {
        cout << "Enter the correct answer index (1-4): ";
        cin >> correctAnswer;
    } while (correctAnswer < 1 || correctAnswer > 4);
    quiz.questions[questionIndex].correctAnswerIndex = correctAnswer - 1;

    // Write the modified quiz data back to the file
    if (!writeQuizData(quiz, quiz.questions, this->courseID)) {
        cerr << "Error: Could not write modified quiz data to file." << endl;
    } else {
        cout << "Quiz " << quizName << " modified successfully!" << endl;
    }
}

void Teacher::displayQuizzes() const {
    string quizzesFilename = this->courseID + ".txt";
    ifstream infile(quizzesFilename.c_str());
    if (!infile.is_open()) {
        cerr << "Error: Could not open file " << quizzesFilename << endl;
        return;
    }

    string quizName;
    cout << "\nAvailable Quizzes:\n";
    while (getline(infile, quizName)) {
        cout << "- " << quizName << endl;
    }
    infile.close();
}

// Function to check if a user exists in the file
// bool userExists(const string& filename, const string& username) {
//     ifstream infile(filename.c_str());
//     if (!infile.is_open()) {
//         cerr << "Error: Could not open file " << filename << endl;
//         return false;
//     }

//     string line;
//     while (getline(infile, line)) {
//         istringstream iss(line);
//         string readUsername;
//         getline(iss, readUsername, ',');

//         if (username == readUsername) {
//             infile.close();
//             return true; // User exists
//         }
//     }

//     infile.close();
//     return false; // User doesn't exist
// }

int main() {
    const string userFilename = "users.txt";
    const string quizFilename = "quizzes.txt";

    int choice, choice_2;

    // Loop to display menu until user quits
    do {
        system("cls"); // Clear the console for a clean look

        // Main Menu
        cout << "\n\n\t\t==============================" << endl;
        cout << "\t\t|| QUIZ MANAGEMENT SYSTEM ||" << endl;
        cout << "\t\t==============================" << endl << endl;

        cout << "\t\t1. Login" << endl;
        cout << "\t\t2. Signup" << endl;
        cout << "\t\t3. Exit" << endl;
        cout << "\n\t\tEnter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1: {
            string username, password;
            cout << "\n\t\tEnter username: ";
            cin >> username;
            cout << "\t\tEnter password: ";
            cin >> password;

            User user = readUserData(userFilename, username, password);
            if (user.getUsername().empty()) {
                cout << "\n\t\tInvalid username or password." << endl;
                system("pause"); // Pause for the user to see the error
            } else {
                // Handle successful login based on user type (Teacher or Student)
                if (user.getDesignation() == "teacher" || user.getDesignation() == "Teacher") {
                    // Teacher functionalities (create quiz, etc.)
                    cout << "\n\t\tWelcome, Teacher " << user.getUsername() << endl;
                    system("pause");
                    system("cls"); // Clear the console again

                    // Teacher Menu
                    do {
                        cout << "\n\n\t\t==============================" << endl;
                        cout << "\t\t||    TEACHER MENU    ||" << endl;
                        cout << "\t\t==============================" << endl << endl;

                        cout << "\t\t1. Create Quiz" << endl;
                        cout << "\t\t2. Modify Quiz" << endl;
                        cout << "\t\t3. View Quizzes" << endl; 
                        cout << "\t\t4. Exit" << endl;
                        cout << "\n\t\tEnter your choice: ";
                        cin >> choice_2;

                        switch (choice_2) {
                        case 1: {
                            string name_quiz;
                            int holder;
                            cout << "\n\t\tPlease enter name of Quiz: ";
                            cin >> name_quiz;

                            // Checking to see if quiz of that name already exists
                            if (quizExists(user.getCourseID(), name_quiz)) {
                                cout << "\n\t\tQuiz '" << name_quiz << "' already exists." << endl;
                                system("pause"); // Pause for the user to see the error
                                break; 
                            }

                            cout << "\n\t\tPlease enter number of questions: ";
                            cin >> holder;

                            Teacher teacher(user.getUsername(), user.getPassword(), user.getDesignation(), user.getCourseID());
                            teacher.createQuiz(name_quiz, holder);
                            break;
                        }
                        case 2: {
                            string quizName;
                            cout << "\n\t\tEnter the name of the quiz you want to modify: ";
                            cin >> quizName;
                            Teacher teacher(user.getUsername(), user.getPassword(), user.getDesignation(), user.getCourseID());
                            teacher.modifyQuiz(quizName);
                            break;
                        }
                        case 3: {
                            Teacher teacher(user.getUsername(), user.getPassword(), user.getDesignation(), user.getCourseID());
                            teacher.displayQuizzes();
                            system("pause"); // Pause for the user to see the quizzes
                            break;
                        }
                        case 4: {
                            cout << "\n\t\tExiting Teacher Menu..." << endl;
                            break;
                        }
                        default:
                            cout << "\n\t\tInvalid choice. Please try again." << endl;
                            system("pause");
                        }
                    } while (choice_2 != 4);
                } else {
                    // Student Menu
                    cout << "\n\t\tWelcome, Student " << user.getUsername() << endl;
                    system("pause");
                    system("cls"); // Clear the console again

                    do {
                        cout << "\n\n\t\t==============================" << endl;
                        cout << "\t\t||    STUDENT MENU    ||" << endl;
                        cout << "\t\t==============================" << endl << endl;

                        // Show available quizzes
                        string holder;
                        ifstream infile((user.getCourseID() + ".txt").c_str());
                        if (!infile.is_open()) {
                            cerr << "Error: Could not open file. There are no available quizzes for this course.";
                        } else {
                            cout << "\t\tAvailable quizzes are: " << endl << endl;

                            while (!infile.eof()) {
                                getline(infile, holder);
                                cout << "\t\t- " << holder << endl;
                            }
                        }
                        infile.close();

                        // Prompt user to choose a quiz
                        string chosenQuizName;
                        cout << "\n\t\tEnter the name of the quiz you want to take (or 'exit' to quit): ";
                        cin >> chosenQuizName;

                        // Checking to see if quiz of that name already exists
                        string getattempted = user.getCourseID() + "_" + user.getUsername() + ".txt";
                        string name_holder;
                        string line;
                        string quiz_grade;

                        ifstream quizzes(getattempted.c_str());

                        if (!quizzes.is_open()) {
                            // File doesn't exist, quiz hasn't been attempted.
                        } else {
                            while (getline(quizzes, line)) {
                                istringstream check(line);
                                // Compare only the name_quiz with the extracted name
                                getline(check, name_holder, ',');
                                getline(check, quiz_grade, ',');

                                if (chosenQuizName == name_holder) {
                                    cout << "\n\t\tQuiz '" << chosenQuizName << "' already attempted." << "Grade is: " << quiz_grade;
                                    quizzes.close(); // Close the file after finding a match
                                    system("pause");
                                    return 0;
                                    break; // Exit the loop
                                }
                            }
                        }
                        quizzes.close();

                        if (chosenQuizName != "exit") {
                            // Read quiz data from file
                            Quiz quiz = readQuizData(user.getCourseID(), chosenQuizName);
                            if (quiz.name.empty()) {
                                cerr << "Error: Could not find quiz " << chosenQuizName << endl;
                            } else {
                                // Let the student take the quiz
                                Student student(username, password, "student", user.getCourseID());
                                student.takeQuiz(quiz, user);
                                system("pause");
                            }
                        } else {
                            break;
                        }
                    } while (true);
                }
            }
            break;
        }
        case 2: {
            string username, password, designation;
            string courseID;
            cout << "\n\t\tEnter username: ";
            cin >> username;

            // Check if user already exists
            // if (userExists(userFilename, username)) {
            //     cout << "\n\t\tUser with this username already exists!" << endl;
            //     system("pause");
            //     break; // Skip to the next case
            // }

            cout << "\n\t\tEnter password: ";
            cin >> password;
            cout << "\n\t\tEnter Designation (teacher/student): ";
            cin >> designation;
            cout << "\n\t\tEnter CourseID: ";
            cin >> courseID;

            User user(username, password, designation, courseID);
            if (writeUserData(user, userFilename)) {
                cout << "\n\t\tSignup successful!" << endl;
                system("pause");
            } else {
                cout << "\n\t\tError creating user account." << endl;
                system("pause");
            }
            break;
        }
        case 3:
            cout << "\n\t\tExiting Quiz Management System..." << endl;
            system("pause");
            break;
        default:
            cout << "\n\t\tInvalid choice. Please try again." << endl;
            system("pause");
        }

    } while (choice != 3);

    return 0;
}