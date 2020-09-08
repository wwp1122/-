#pragma once

#include <QWidget>
#include "ui_CenterWidget.h"

#include <set>
#include <vector>
#include <stack>
#include <QThread>
#include <QMouseEvent>

#if defined(_MSC_VER) && (_MSC_VER >= 1600)    
# pragma execution_character_set("utf-8")    
#endif

enum MousePressResult
{
	CLICK_NULL,
	CLICK_MINE,
	CLICK_MINE_AROUND,
};
struct MousePressResultValue
{
	int pressType;
	std::set<std::pair<int, int> > result;
	int aroundMineCount;
};
Q_DECLARE_METATYPE(MousePressResultValue);


class EventWorker;
class CenterWidget : public QWidget
{
	Q_OBJECT

public:
	CenterWidget(QWidget *parent = Q_NULLPTR);
	~CenterWidget();

	void setMainLayout(const int& x, const int& y);
private:
	QRect getRectFromPos(const int& x, const int& y);

	Ui::CenterWidget ui;

	int xSize;
	int ySize;
	int invertalW;
	int invertalH;
	int xPos;
	int yPos;
	QVector<QPoint> allLines;
	EventWorker* eventWorker; 
	QThread* thread;

	std::map<std::pair<int, int>, QString>currentCondition;
protected:
	void paintEvent(QPaintEvent* event);
	void mousePressEvent(QMouseEvent* mEvent);
signals:
	void startCalculate();
	void mpusePress();
	void lastMineCountChange(const int& i);
public slots:
	void getMousePressResult(const QVariant& value);
	//const MousePressResult& pressType,std::vector<std::vector<int> > result
};

class EventWorker : public QObject
{
	Q_OBJECT
public:
	EventWorker();
	~EventWorker();

	void setGrid(const int& xSi, const int& ySi);
	void setMousePos(const int& xPos, const int& yPos);
	int** getAllMine();

	void start();
	void stop();
private:
	void getRandomInt(int &first, int&second, const int& xMax, const int& yMax);
	void calculateArray();
	void getAllNullPos(std::stack<std::pair<int, int> >& stack, std::set<std::pair<int, int> >&result);


	int xSize;
	int ySize;
	int xPosition;
	int yPosition;
	int allPosMineCount[1000][1000];
	std::set<std::pair<int, int> > allMines;

	int isRunning;
signals:
	void mousePressResult(const QVariant& value);
public slots:
	void calculateMinesPos();
	void calculateMousePressResult();
};