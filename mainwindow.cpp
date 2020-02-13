#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);    
    initAll();
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::initAll()
{
    this->setWindowTitle("Learn to type");
    this->setFixedSize(this->size());

    connect();
    ui->lineEdit_WPM->setReadOnly(true);
    ui->lineEdit_Row->setDisabled(true);
    ui->lineEdit_Row->setStyleSheet("color: black; border-style: solid; border-width: 1px; border-color: black;");
    maxRandomChance = 3;
    generator = new QRandomGenerator(QRandomGenerator::securelySeeded());

    setTimer();
    showTables();
    symbols1 = {"`","-","=","[","]","\\",";","'",",",".","/"};
    symbols2 = {"~","!","@","#","$","%","^","&","*","(",")","_","+","{","}","|",":","\"","<",">","?"};
    symbols = symbols1 + symbols2;
    symbols.append({"+=", "==", "-+", "/=", "*=", "()", "{}", "[]", "<>", "</", "!="});
    ListOfPLCharsUp = {"Ą", "Ć", "Ę", "Ł", "Ń", "Ó", "Ś", "Ź", "Ż"};
    ListOfPLCharsLow = {"ą", "ć", "ę", "ł", "ń", "ó", "ś", "ź", "ż"};

    whiteSeparator = "   ";
    fm = new QFontMetrics(ui->lineEdit_Row->font());
    int height = fm->height();
    double heightCoefficient = 1.4;
    int itemHeight = static_cast<int>(heightCoefficient * height);
    ui->comboBox_TimerSelect->setFixedHeight(itemHeight);//
    ui->lineEdit_Row->setFixedHeight(itemHeight);
    ui->lineEdit_WriteTo->setFixedHeight(itemHeight);
    ui->lineEdit_Timer->setFixedHeight(itemHeight);
    ui->btnStart->setFixedHeight(itemHeight);
    ui->spinBox_From->setMinimum(1);
    ui->spinBox_To->setMinimum(1);
    ui->spinBox_NumberChance->setMaximum(maxRandomChance);
    ui->spinBox_SymbolChance->setMaximum(maxRandomChance);
    ui->spinBox_UpLetterChance->setMaximum(maxRandomChance);
    ui->spinBox_NumberChance->setValue(maxRandomChance);
    ui->spinBox_SymbolChance->setValue(maxRandomChance);
    ui->spinBox_UpLetterChance->setValue(maxRandomChance - 1);
    ui->spinBox_To->setValue(4);
    ui->lineEdit_WPM->setReadOnly(true);
    ui->lineEdit_Timer->setReadOnly(true);
    ui->lineEdit_Correct->setReadOnly(true);
    ui->lineEdit_Incorrect->setReadOnly(true);
    ui->lineEdit_Accuracy->setReadOnly(true);

    ui->comboBox_TimerSelect->addItem("0:30");
    ui->comboBox_TimerSelect->addItem("1:00");
    ui->comboBox_TimerSelect->addItem("1:30");
    ui->comboBox_TimerSelect->addItem("2:00");
    ui->comboBox_TimerSelect->addItem("3:00");
    ui->comboBox_TimerSelect->addItem("5:00");
    ui->comboBox_TimerSelect->addItem("10:00");
    ui->comboBox_TimerSelect->setCurrentIndex(3);

    ui->lineEdit_Timer->setAlignment(Qt::AlignCenter);
    setLineEdit(ui->lineEdit_Correct, 0);
    setLineEdit(ui->lineEdit_Incorrect, 0);
    setLineEdit(ui->lineEdit_Accuracy, 0);
    setLineEdit(ui->lineEdit_WPM, 0);

    ui->btnStart->setFocus();
}
void MainWindow::connect()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    QString connectionString = QDir::currentPath() + "/Dictionaries.db";
    db.setDatabaseName(connectionString);
    if (!db.open())
    {
        qDebug() << db.lastError().text();
    }
}



void MainWindow::addTable()
{
    QString name = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                         tr("Name:"), QLineEdit::Normal);

    QSqlQuery query("CREATE TABLE \""+name+"\"(word varchar(30) PRIMARY KEY)");
    showTables();
}
void MainWindow::showTables()
{
    ui->comboBox_Dictionaries->clear();
    QSqlQuery query("SELECT name FROM sqlite_master WHERE type ='table' AND name NOT LIKE 'sqlite_%'ORDER BY name;");
    int name = query.record().indexOf("name");
    int count = 1;
    while(query.next())
    {
        QString str = QString::number(count) + ". " + query.value(name).toString();
        ui->comboBox_Dictionaries->addItem(str);
        count++;
    }

    ui->comboBox_Dictionaries->addItem("Numbers");
    ui->comboBox_Dictionaries->addItem("Symbols");
    ui->comboBox_Dictionaries->addItem("UpLetters");
    showWords();
}

void MainWindow::addWords()
{
    auto table = tableName();
    if(table.isEmpty()) return;
    QString text = QInputDialog::getMultiLineText(this, tr("Add words"), tr("Words:"));
    QList<QString> listOfWords;
    QString word ="";
    for (int i=0; i<text.length();i++)
    {
        if(text[i].isUpper())
        {
            if(!text[i+1].isUpper())
            {
                if(i==0)
                    text[i] = text[i].toLower();
                else if(!text[i-1].isUpper())
                {
                    text[i] = text[i].toLower();
                }
            }
        }

        if((text[i] >= 97 && text[i] <= 122) || (text[i] >= 65 && text[i] <= 90) || text[i] == 39)
            word += text[i];
        else
        {
            bool WordsEnd = true;
            for (int j = 0; j < ListOfPLCharsUp.length(); j++)
            {
                if(text.mid(i,1) == ListOfPLCharsUp[j])
                {
                    //KONWERSJA Z DUŻYCH NA MAŁE
                    for (int k = 0; k < ListOfPLCharsUp.length(); k++)
                        if(text.mid(i + 1, 1) != ListOfPLCharsUp[k])
                        {
                            if(i==0)
                                text.mid(i, 1) = ListOfPLCharsLow[k];
                            else
                            {
                                for (int l = 0; l < ListOfPLCharsUp.length(); l++)
                                    if(text.mid(i - 1, 1) != ListOfPLCharsUp[k])
                                        text.mid(i, 1) = ListOfPLCharsLow[k];
                            }
                        }
                    //KONIEC

                    word += text.mid(i,1);
                    WordsEnd = false;
                }
            }
            for (int j = 0; j < ListOfPLCharsLow.length(); j++)
            {
                if(text.mid(i,1) == ListOfPLCharsLow[j])
                {
                    word += text.mid(i,1);
                    WordsEnd = false;
                }
            }
            if(word!="" && word!="\n" && WordsEnd)
            {
                listOfWords.append(word);
                word = "";
            }
        }
    }
    if(word!="" && word!="\n")
        listOfWords.append(word);
    ui->comboBox_Content->clear();
    for (QString str: listOfWords)
        ui->comboBox_Content->addItem(str);
    //INSERT    
    for(auto word : listOfWords)
    {
        QSqlQuery query;
        query.prepare("INSERT INTO \"" +  table + "\"(word) "
                          "VALUES (:word)");
            query.bindValue(":word", word);
            query.exec();
    }
    showWords();
}


void MainWindow::showWords()
{
    ui->comboBox_Content->clear();
    if(ui->comboBox_Dictionaries->currentText() == "Numbers")
    {
        for(int i = 0; i < 10; i++)
            ui->comboBox_Content->addItem(QString::number(i));
        countOfWords = 10;
        ui->label_Content->setText("Content(" + QString::number(countOfWords) + ")");
        return;
    }
    if(ui->comboBox_Dictionaries->currentText() == "Symbols")
    {
        for(auto symbol : symbols)
            ui->comboBox_Content->addItem(symbol);
        countOfWords = symbols.length();
        ui->label_Content->setText("Content(" + QString::number(countOfWords) + ")");
        return;
    }
    if(ui->comboBox_Dictionaries->currentText() == "UpLetters")
    {
        for(int i = 65; i <= 90; i++)
            ui->comboBox_Content->addItem(QChar(i));
        for(auto letter : ListOfPLCharsUp)
            ui->comboBox_Content->addItem(letter);
        countOfWords = 35;
        ui->label_Content->setText("Content(" + QString::number(countOfWords) + ")");
        return;
    }

    countOfWords = countWords();

    auto table = tableName();
    if(table.isEmpty()) return;

    QSqlQuery query("SELECT * FROM \"" + table + "\" ORDER BY word");
    int word = query.record().indexOf("word");
    while(query.next())
    {
        QString str = query.value(word).toString();
        ui->comboBox_Content->addItem(str);
    }
}


void MainWindow::renameTable()
{
        auto table = tableName();
        if(table.isEmpty()) return;
        QString newName = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                             tr("New Name:"), QLineEdit::Normal, table);
        QSqlQuery query("ALTER TABLE \"" + table + "\" RENAME TO \"" + newName + "\"");
        showTables();
}

void MainWindow::deleteTable()
{
    auto table = tableName();
    if(table.isEmpty()) return;
    QSqlQuery query("DROP TABLE \"" + tableName() + "\"");
    showTables();
}

void MainWindow::changeWord()
{
    auto table = tableName();
    if(table.isEmpty()) return;
    auto word = ui->comboBox_Content->currentText();
    QString newWord = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                         tr("Edit:"), QLineEdit::Normal, word);
    if(newWord.isEmpty()) return;
    QString querryString("UPDATE \"" + table + "\" SET WORD = '" + newWord + "'WHERE WORD = \"" + word + "\"");
    QSqlQuery query(querryString);
    showWords();
}

void MainWindow::deleteWord()
{
    auto table = tableName();
    if(table.isEmpty()) return;
    auto word = ui->comboBox_Content->currentText();

    QSqlQuery query("DELETE FROM \"" + table + "\" WHERE WORD = \"" + word +"\"");
    showWords();
}

QString MainWindow::tableName()
{
    auto tableTemp = ui->comboBox_Dictionaries->currentText();
    if(tableTemp == "Numbers" || tableTemp == "Symbols" || tableTemp == "UpLetters" || tableTemp.isEmpty()) return "";
    bool write = false;
    QString table = "";
    for (auto c :tableTemp)
    {
        if(write)
            table += c;
        if(c == ' ')
            write = true;
    }
    return table;
}

void MainWindow::on_btnStart_clicked()
{
    start();
}
void MainWindow::start()
{
    if(countOfWords == 0)
        return;
    writeToActive = true;
    if(timer->isActive())
        timer->stop();
    ui->lineEdit_WriteTo->clear();
    ui->lineEdit_WriteTo->setFocus();
    timerOn = false;
    ui->lineEdit_Timer->setText("0:00");
    setLineEdit(ui->lineEdit_Correct, 0);
    setLineEdit(ui->lineEdit_Incorrect, 0);
    setLineEdit(ui->lineEdit_Accuracy, 0);
    setLineEdit(ui->lineEdit_WPM, 0);
    allKeystrokes = 0;
    correctKeystrokes = 0;
    correctWords = 0;
    incorrectWords = 0;
    accuracy = 0;
    maxCursorPosition = 0;
    row = "";
    if(ui->comboBox_Dictionaries->currentText() == "Symbols")
        generateSymbols();
    else if(ui->comboBox_Dictionaries->currentText() == "Numbers")
        generateNumbers();
    else if(ui->comboBox_Dictionaries->currentText() == "UpLetters")
        generateLetters();
    else
        writeWords();
    currentWordValue = evaluateAWord();
    currentWordValueWithAccuracy = 0;
    visited = false;
    correctKeystrokesWithoutAccuracy = 0;
}
int MainWindow::evaluateAChar(QChar c)
{
    if((c >= 97 && c <= 122) || (c >= 48 && c <= 57))//MAŁE LITERY I CYFRY
        return 1;
    for (int j = 0; j < symbols1.length(); j++)//SYMBOLE 1
        if (c == symbols1[j])
            return 1;
    for (int j = 0; j < ListOfPLCharsUp.length(); j++)//POLSKIE DUŻE LITERY
        if (c == ListOfPLCharsUp[j])
            return 3;
    if (c == '~')
        return 3;
//RESZTA: POLSKIE MAŁE LITERY, SYMBOLE 2
    return 2;
}
int MainWindow::evaluateAWord()
{
    endOfWordIndex = 0;
    int i=0;
    int value = 0;
    while(row.at(i) != ' ')//WORD VALUE
    {
        endOfWordIndex++;
        value += evaluateAChar(row.at(i));
        i++;
    }
    return value;
}
void MainWindow::on_lineEdit_WriteTo_textChanged(const QString &arg1)
{
    if(row.isEmpty() || !writeToActive) return;
    if(!timerOn)
    {
        seconds = 0;
        timer->start(1000);
        timerOn = true;
    }
    if(!timer->isActive()) return;
    int currentCursorPosition = ui->lineEdit_WriteTo->cursorPosition() - 1;
    if(currentCursorPosition > maxCursorPosition)
    {
        maxCursorPosition = currentCursorPosition;
        visited = false;
    }


    QChar currentChar;
    bool correct = true;
    if(!arg1.isEmpty())
    {
        QPalette *palette = new QPalette();
        currentChar = arg1.at(currentCursorPosition);

        if(currentChar == row.at(currentCursorPosition) && currentCursorPosition == maxCursorPosition && currentCursorPosition < endOfWordIndex && !visited)
            currentWordValueWithAccuracy += evaluateAChar(row.at(currentCursorPosition));
        visited = true;

        if(currentCursorPosition >= endOfWordIndex && currentCursorPosition == maxCursorPosition && currentChar != ' ')
            allKeystrokes++;

        for(int i=0; i<=currentCursorPosition; i++)
            if(arg1.at(i) != row.at(i))
                correct = false;
        if(correct)
           palette->setColor(QPalette::Text,QColor(0,150,0));
        else
            palette->setColor(QPalette::Text,Qt::red);
        ui->lineEdit_WriteTo->setPalette(*palette);
    }

    if(currentChar != ' ') return;
    int i = 0;
    while(row.at(i) != ' ' && ui->lineEdit_WriteTo->text().at(i) != ' ')
    {
        if(ui->lineEdit_WriteTo->text().at(i) == row.at(i))
            correctKeystrokesWithoutAccuracy += evaluateAChar(row.at(i));
        i++;
    }


    bool isSpace = false;
    bool writeRow = false;
    QString newRow = "";
    for (int i=0; i<row.length(); i++)
    {
        if(isSpace && row.at(i) != ' ')
            writeRow = true;
        if(writeRow)
            newRow += row.at(i);
        if(row.at(i) == ' ')
            isSpace = true;
    }
    row = newRow;

    if(ui->comboBox_Dictionaries->currentText() == "Numbers")
        generateNumbers();
    else if(ui->comboBox_Dictionaries->currentText() == "Symbols")
        generateSymbols();
    else if(ui->comboBox_Dictionaries->currentText() == "UpLetters")
        generateLetters();
    else
        generateWords();

    allKeystrokes += currentWordValue;
    correctKeystrokes += currentWordValueWithAccuracy;

    if(correct)
        correctWords++;
    else
        incorrectWords++;
    visited = false;
    maxCursorPosition = 0;
    setLineEdit(ui->lineEdit_Correct, correctWords);
    setLineEdit(ui->lineEdit_Incorrect, incorrectWords);

    if(allKeystrokes != 0)
    {
        accuracy = static_cast<double>(correctKeystrokes)/allKeystrokes * 100.0;
        setLineEdit(ui->lineEdit_Accuracy, accuracy);
    }

    currentWordValue = evaluateAWord();
    currentWordValueWithAccuracy = 0;
    ui->lineEdit_WriteTo->clear();
}

void MainWindow::setLineEdit(QLineEdit *lineEdit, double value)
{
    fm = new QFontMetrics(lineEdit->font());
    int width = 190;
    QString name = "";
    QString valueStr = QString::number(value);

    if(lineEdit->objectName() == "lineEdit_Correct")
        name = "Correct Words:";
    if(lineEdit->objectName() == "lineEdit_Incorrect")
        name = "Incorrect Words:";
    if(lineEdit->objectName() == "lineEdit_WPM")
        name = "WPM:";
    if(lineEdit->objectName() == "lineEdit_Accuracy")
    {
        valueStr = QString::number(value, 'f', 1);
        name = "Accuracy:";
    }

    int currentWidth = fm->width(name);
    int spaceCount = 0;
    while(currentWidth < width)
    {
        currentWidth += fm->width(' ');
        spaceCount++;
    }

    if(lineEdit->objectName() == "lineEdit_Accuracy")
    {
        QString accuracyStr = QString::number(value, 'f', 1);
        lineEdit->setText(QString(name + "%1%").arg(valueStr, spaceCount - 1));
    }
    else
        lineEdit->setText(QString(name + "%1").arg(valueStr, spaceCount));
}

int MainWindow::countWords()
{
    auto table = tableName();
    if(table.isEmpty()) return 0;
    QSqlQuery query("SELECT COUNT (*) FROM \"" + table + "\"");
    int count = 0;
    if (query.next())
           count = query.value(0).toInt();
    ui->label_Content->setText("Content(" + QString::number(count) + ")");
    return count;
}
void MainWindow::writeWords()
{
    if(ui->comboBox_Dictionaries->currentText().isEmpty()) return;
    listOfWords.clear();

    auto table = tableName();

    if(countOfWords == 0)
    {
        ui->lineEdit_Row->clear();
        return;
    }

    QSqlQuery query2("SELECT * FROM \"" + table + "\"");
    int IndexWord = query2.record().indexOf("word");
    while(query2.next())
    {
        QString str = query2.value(IndexWord).toString();        
        listOfWords.append(str);
    }
    if(countOfWords != 0)
        generateWords();
    else
        ui->lineEdit_WriteTo->setStyleSheet("color: black;");

}

void MainWindow::generateWords()//GENERUJE SŁOWA
{
    fm = new QFontMetrics(ui->lineEdit_Row->font());
    maxWidth = ui->lineEdit_Row->width() - 5;//MAX WIDTH
    double width = 0;
    while(true)
    {
        width = fm->width(row);
        width += RandomSymbol();

        int i = QRandomGenerator::global()->bounded(countOfWords);
        QString newWord = listOfWords.at(i);
        if(generator->bounded((maxRandomChance + 1 - ui->spinBox_UpLetterChance->value())) == 0 && newWord.at(0).isLower() &&
                ui->spinBox_UpLetterChance->value() != 0)
        {
           QString UpWord = "";
           UpWord.append(newWord.at(0).toUpper());
           UpWord.append(newWord.mid(1));
           newWord = UpWord;
        }
        if(width + fm->width(newWord) + fm->width(whiteSeparator) > maxWidth)
        {
            ui->lineEdit_Row->setText(row);
            return;
        }
        row += newWord;
        width += RandomSymbol();
        row += whiteSeparator;
        width += RandomNumber();
    }
}
int MainWindow::RandomSymbol()
{
    if(ui->spinBox_SymbolChance->value() == 0) return 0;
    fm = new QFontMetrics(ui->lineEdit_Row->font());
    maxWidth = ui->lineEdit_Row->width() - 5;
    int width = fm->width(row);

    QString symbol = symbols.at(generator->bounded(symbols.length()));
    if(generator->bounded((maxRandomChance + 1 - ui->spinBox_SymbolChance->value())) == 0 && width + fm->width(symbol) + fm->width(whiteSeparator) < maxWidth)
    {
        row += symbol;
        return fm->width(symbol);
    }
    return 0;
}

int MainWindow::RandomNumber()
{
    if(ui->spinBox_NumberChance->value() == 0) return 0;
    int min = ui->spinBox_From->value();
    int max = ui->spinBox_To->value();
    fm = new QFontMetrics(ui->lineEdit_Row->font());
    maxWidth = ui->lineEdit_Row->width() - 5;
    int width = fm->width(row);
    int length = generator->bounded(max + 1 - min) + min;
    QString number = "";
    for(int i = 0; i < length; i++)
        number += QString::number(generator->bounded(10));

    if(generator->bounded((maxRandomChance + 1 - ui->spinBox_NumberChance->value())) == 0 && width + fm->width(number) + fm->width(whiteSeparator) < maxWidth)
    {
        row += number + whiteSeparator;
        ui->lineEdit_Row->setText(row);
        return fm->width(number);
    }
    return 0;
}


void MainWindow::setTimer()
{
    timer = new QTimer(this);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(timerAction()));
}

void MainWindow::timerAction()
{
    seconds++;
    int min = seconds / 60;
    int sec = (seconds % 60);
    QString time = QString::number(min) + ':' + QString::number(sec).rightJustified(2, '0');

    double KPM = correctKeystrokesWithoutAccuracy/(seconds/60.0);
    int WPM = static_cast<int>(KPM/5);
    setLineEdit(ui->lineEdit_WPM, WPM);
    ui->lineEdit_Timer->setText(time);
    bool ok;
    QStringList list = ui->comboBox_TimerSelect->currentText().split(':');
    int minEnd = list[0].toInt(&ok) * 60;
    int secEnd = list[1].toInt(&ok);
    int TimeEnd = minEnd + secEnd;
    if(seconds == TimeEnd)
    {
        stopTimer();
    }

}
void MainWindow::stopTimer()
{
    writeToActive = false;
    if(!timer->isActive()) return;
    timer->stop();
    QPalette *palette = new QPalette();
    palette->setColor(QPalette::Text,Qt::black);
    ui->lineEdit_WriteTo->setPalette(*palette);
    seconds = 0;
}

void MainWindow::generateNumbers()
{
    int min = ui->spinBox_From->value();
    int max = ui->spinBox_To->value();
    fm = new QFontMetrics(ui->lineEdit_Row->font());
    maxWidth = ui->lineEdit_Row->width() - 5;
    while(true)
    {
        int width = fm->width(row);
        int length = generator->bounded(max + 1 - min) + min;
        QString number = "";
        for(int i = 0; i < length; i++)
            number += QString::number(generator->bounded(10));
        if(width + fm->width(number) + fm->width(whiteSeparator) > maxWidth) break;
        row += number + whiteSeparator;
    }
    ui->lineEdit_Row->setText(row);
}

void MainWindow::generateSymbols()
{
    int min = ui->spinBox_From->value();
    int max = ui->spinBox_To->value();
    fm = new QFontMetrics(ui->lineEdit_Row->font());
    maxWidth = ui->lineEdit_Row->width() - 5;
    while(true)
    {
        int width = fm->width(row);
        int length = generator->bounded(max + 1 - min) + min;
        if(length == 0)return;
        QString groupOfSymbols = "";
        for(int i = 0; i < length; i++)
        {
            int j = generator->bounded(symbols.length());
            if(symbols.at(j).length() > 1)
            {
                i++;
                if(i == length)
                {
                    groupOfSymbols += symbols.at(j).at(0);
                    break;
                }
            }
            groupOfSymbols += symbols.at(j);
        }
        if(width + fm->width(groupOfSymbols) + fm->width(whiteSeparator) > maxWidth) break;
        row += groupOfSymbols + whiteSeparator;
    }
    ui->lineEdit_Row->setText(row);
}

void MainWindow::generateLetters()
{
    int min = ui->spinBox_From->value();
    int max = ui->spinBox_To->value();
    fm = new QFontMetrics(ui->lineEdit_Row->font());
    maxWidth = ui->lineEdit_Row->width() - 5;
    while(true)
    {
        int width = fm->width(row);
        int length = generator->bounded(max + 1 - min) + min;

        QString letters = "";
        for(int i = 0; i < length; i++)
        {
            int j = generator->bounded(91 + ListOfPLCharsUp.length() - 65);
            if(j + 65 <= 90)
                letters += QChar(j + 65);
            else
                letters += ListOfPLCharsUp.at(j - (90 - 65) - 1);
        }
        if(width + fm->width(letters) + fm->width(whiteSeparator) > maxWidth) break;
        row += letters + whiteSeparator;
    }
    ui->lineEdit_Row->setText(row);
}

void MainWindow::on_spinBox_From_valueChanged()
{
    if(ui->spinBox_From->value() > ui->spinBox_To->value())
         ui->spinBox_To->setValue(ui->spinBox_From->value());
}

void MainWindow::on_spinBox_To_valueChanged()
{
    if(ui->spinBox_To->value() < ui->spinBox_From->value())
         ui->spinBox_From->setValue(ui->spinBox_To->value());
}

void MainWindow::keyPressEvent(QKeyEvent * event)
{
    if (event->key() == Qt::Key_F5)
        start();
}

void MainWindow::on_actionAdd_Words_triggered()
{
    stopTimer();
    addWords();
}
void MainWindow::on_actionAdd_Dictionary_triggered()
{
    stopTimer();
    addTable();
}

void MainWindow::on_actionEdit_triggered()
{
    stopTimer();
    changeWord();
}

void MainWindow::on_actionDeleteWord_triggered()
{
    stopTimer();
    deleteWord();
}

void MainWindow::on_actionRename_triggered()
{
    stopTimer();
    renameTable();
}

void MainWindow::on_actionDeleteTable_triggered()
{
    stopTimer();
    deleteTable();
}

void MainWindow::on_comboBox_Dictionaries_currentIndexChanged()
{
    stopTimer();
    showWords();
}
