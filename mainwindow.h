#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QSqlRecord>
#include <QInputDialog>
#include <QList>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QRandomGenerator>
#include <QTimer>
#include <QFontMetrics>
#include <QColor>
#include <QPalette>
#include <QKeyEvent>
#include <QDir>
#include <QRandomGenerator>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void connect();
    void showTables();
    void start();

private slots:

    void initAll();

    void renameTable();

    void deleteTable();

    void changeWord();

    void deleteWord();

    QString tableName();

    void on_btnStart_clicked();

    void writeWords();

    void on_lineEdit_WriteTo_textChanged(const QString &arg1);

    void generateWords();

    void setTimer();

    void timerAction();

    int RandomSymbol();

    int RandomNumber();

    void generateNumbers();

    void generateSymbols();

    void on_spinBox_From_valueChanged();

    void on_spinBox_To_valueChanged();

    void showWords();

    void generateLetters();

    void keyPressEvent(QKeyEvent *event);

    void on_actionAdd_Words_triggered();

    void addWords();

    void on_actionAdd_Dictionary_triggered();

    void addTable();

    void on_actionEdit_triggered();

    void on_actionDeleteWord_triggered();

    void on_actionRename_triggered();

    void on_actionDeleteTable_triggered();

    void stopTimer();

    int countWords();

    int evaluateAChar(QChar);

    int evaluateAWord();

    void setLineEdit(QLineEdit*, double);

    void on_comboBox_Dictionaries_currentIndexChanged();

private:
    Ui::MainWindow *ui;
    QSqlDatabase db;
    QString newDbName;
    QString row;
    QList<QString> listOfWords;
    int countOfWords;
    int seconds;
    QTimer *timer;
    int allKeystrokes = 0;
    int correctKeystrokes = 0;
    int correctWords = 0;
    int incorrectWords = 0;
    bool timerOn = false;
    QList<QString> symbols;
    QList<QString> symbols1;
    QList<QString> symbols2;
    QList<QString> ListOfPLCharsUp;
    QList<QString> ListOfPLCharsLow;
    QFontMetrics *fm;
    int maxWidth;
    int currentWordValueWithAccuracy = 0;
    int currentWordValue = 0;
    double accuracy = 0;
    int maxCursorPosition;
    int endOfWordIndex;
    bool visited;
    int correctKeystrokesWithoutAccuracy = 0;
    int maxRandomChance;
    QRandomGenerator *generator;
    QString whiteSeparator;
    bool writeToActive = false;

};

#endif // MAINWINDOW_H
