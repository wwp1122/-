#include "CenterWidget.h"
#include <QPainter>
#include <QMessageBox>

CenterWidget::CenterWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	//this->setStyleSheet(QString("border-image:url(:/MineSweeping/backGround);"));

	eventWorker = new EventWorker;
	thread = new QThread();
	eventWorker->moveToThread(thread);
	connect(thread, &QThread::finished, thread, &QThread::deleteLater);
	connect(thread, &QThread::finished, eventWorker, &EventWorker::deleteLater);
	connect(this, &CenterWidget::startCalculate, eventWorker, &EventWorker::calculateMinesPos); 
	connect(this, &CenterWidget::mpusePress, eventWorker, &EventWorker::calculateMousePressResult);
	connect(eventWorker, &EventWorker::mousePressResult, this, &CenterWidget::getMousePressResult);
	thread->start();
}

CenterWidget::~CenterWidget()
{
	eventWorker->stop();
	//delete thread;
}

void CenterWidget::setMainLayout(const int& x, const int& y)
{
	allLines.clear();
	currentCondition.clear();

	int width = this->width();
	int height = this->height();

	xSize = x;
	ySize = y;

	invertalW = (width-4) / (xSize);
	invertalH = (height-4) / (ySize);
	int actualWidth = invertalW * (xSize);
	int actualHeight = invertalH * (ySize);

	QPoint topPoint,buttomPoint,leftPoint,rightPoint;
	for (int i = 0; i <= xSize; ++i)
	{
		topPoint.setX(i * invertalW +2);
		topPoint.setY(0);

		buttomPoint.setX(topPoint.x());
		buttomPoint.setY(actualHeight);

		allLines.push_back(topPoint);
		allLines.push_back(buttomPoint);
 	}
	for (int j = 0; j <= ySize; j++)
	{
		leftPoint.setX(2);
		leftPoint.setY(j * invertalH);

		rightPoint.setX(actualWidth+2);
		rightPoint.setY(leftPoint.y());

		allLines.push_back(leftPoint);
		allLines.push_back(rightPoint);
	}
	update();

	eventWorker->setGrid(xSize, ySize);
	emit startCalculate();
}

QRect CenterWidget::getRectFromPos(const int& x, const int& y)
{
	int leftTopX = x * invertalW + 2;
	int leftTopY = y * invertalH;

	QRect rect(leftTopX,leftTopY, invertalW, invertalH);
	return rect;
}


void CenterWidget::paintEvent(QPaintEvent* event)
{
	QPainter paint(this);
	paint.drawPixmap(QRect(2,0,xSize*invertalW,ySize*invertalH), QPixmap(":/MineSweeping/backGround"), QRect());

	for (auto curCond : currentCondition)
	{
		QRect curRect = getRectFromPos(curCond.first.first, curCond.first.second);
		paint.drawPixmap(curRect, QPixmap(curCond.second), QRect());
	}

	paint.setPen(Qt::black);
	paint.drawLines(allLines);
}
void CenterWidget::mousePressEvent(QMouseEvent* mEvent)
{
	QPoint pos = mEvent->pos();
	int posX = pos.x() - 2;
	int posY = pos.y();

	xPos = posX / invertalW;
	yPos = posY / invertalH;

	if ((xPos >= xSize) || (yPos >= ySize))
	{
		return;
	}
	std::pair<int, int>pair = { xPos ,yPos };
	if (mEvent->button() & Qt::LeftButton)
	{
		if (currentCondition.find(pair) == currentCondition.end())
		{
			eventWorker->setMousePos(xPos, yPos);
			emit mpusePress();
		}
	}
	else if (mEvent->button() & Qt::RightButton)
	{
		auto cond = currentCondition.find(pair);
		if (cond == currentCondition.end())
		{
			QString flagStr(":/MineSweeping/flag");
			currentCondition[pair] = flagStr;
			emit lastMineCountChange(-1);
		}
		else
		{
			if (cond->second == QString(":/MineSweeping/flag"))
			{
				cond->second = QString(":/MineSweeping/question");
				emit lastMineCountChange(1);
			}
			else if (cond->second == QString(":/MineSweeping/question"))
				currentCondition.erase(cond);
		}
		update();
	}
}
void CenterWidget::getMousePressResult(const QVariant& value)
{
	MousePressResultValue pressValue;
	pressValue = value.value<MousePressResultValue>();

	if (CLICK_MINE == pressValue.pressType)
	{
		int** allMineLayout = eventWorker->getAllMine();
		std::pair<int, int> pair;
		int number;
		QString picturePath;
		for (int i = 0; i < xSize; i++)
		{
			for (int j = 0; j < ySize; j++)
			{
				pair.first = i;
				pair.second = j;
				number = allMineLayout[i][j];
				if (9 == number)
					picturePath = QString(":/MineSweeping/mine");
				else if (0 == number)
					picturePath = QString(":/MineSweeping/null");
				else
					picturePath = QString(":/MineSweeping/%1").arg(number);
				currentCondition[pair] = picturePath;
			}
		}
		QMessageBox::information(this, QString("失败！"), QString("错误坐标(%1,%2)").arg(xPos).arg(yPos));
	}
	else if (CLICK_MINE_AROUND == pressValue.pressType)
	{
		std::pair<int, int> pair = { xPos,yPos };
		QString picPath = QString(":/MineSweeping/%1").arg(pressValue.aroundMineCount);
		currentCondition[pair] = picPath;
	}
	else
	{
		//std::pair<int, int> pair = { xPos,yPos };
		for (auto pair : pressValue.result)
		{
			std::pair<int, int> realPair = { pair.first.first - 1,pair.first.second - 1 };
			currentCondition[realPair] = pair.second;
		}
	}

	if (xSize*ySize - currentCondition.size() == xSize * ySize / 10)
	{
		QMessageBox::information(this, QString("成功！"), QString("6"));
	}

	update();
}





EventWorker::EventWorker()
{
	isRunning = true;
}
EventWorker::~EventWorker()
{
	isRunning = false;
}
void EventWorker::start()
{
	isRunning = true;
}
void EventWorker::stop()
{
	isRunning = false;
}

void EventWorker::setGrid(const int& xSi, const int& ySi)
{
	xSize = xSi;
	ySize = ySi;

	allMines.clear();
}
void EventWorker::setMousePos(const int& xPos, const int& yPos)
{
	xPosition = xPos + 1;
	yPosition = yPos + 1;
}
int** EventWorker::getAllMine()
{
	int **dp;
	dp = (int **)malloc(xSize * sizeof(int **));
	for (int i = 0; i < xSize; i++)
		dp[i] = (int *)malloc(ySize * sizeof(int));

	for (int i = 0; i < xSize; i++)
	{
		for (int j = 0; j < ySize; j++)
		{
			dp[i][j] = allPosMineCount[i + 1][j + 1];
		}
	}
	return dp;
}

void EventWorker::getRandomInt(int &first, int&second, const int& xMax, const int& yMax)
{
	first = rand() % xMax;
	second = rand() % yMax;
}
void EventWorker::calculateMinesPos()
{
	if (!isRunning)
		return ;

	int mineCount = xSize * ySize / 10;

	allMines.clear();
	int first, second;
	std::pair<int, int> pair;
	int times = 0;
	srand((int)time(0));
	while (allMines.size() < mineCount)
	{
		getRandomInt(first, second, xSize, ySize);
		pair.first = first;
		pair.second = second;

		allMines.insert(pair);
		++times;
	}

	calculateArray();
}
void EventWorker::calculateArray()
{
	memset(allPosMineCount, 0, sizeof(allPosMineCount));
	for (auto pair: allMines)
		allPosMineCount[pair.first + 1][pair.second + 1] = 9;//所有位置加1	9为该位置存在mine
	int i;
	int j;
	for (i = 0; i <= xSize + 1; ++i)
		allPosMineCount[i][0] = -1;
	for (i = 0; i <= xSize + 1; ++i)
		allPosMineCount[i][ySize+1] = -1;
	for (j = 0; j <= ySize + 1; ++j)
		allPosMineCount[0][j] = -1;
	for (j = 0; j <= ySize + 1; ++j)
		allPosMineCount[xSize + 1][j] = -1;

	for (int i = 1; i < xSize+1; ++i)
	{
		for (int j = 1; j < ySize+1; ++j)
		{
			if(9 == allPosMineCount[i ][j ])
				continue;

			int aroundMinesCount = 0;
			if (9 == allPosMineCount[i - 1][j - 1])
				++aroundMinesCount;
			if (9 == allPosMineCount[i - 1][j])
				++aroundMinesCount;
			if (9 == allPosMineCount[i - 1][j + 1])
				++aroundMinesCount;
			if (9 == allPosMineCount[i ][j - 1])
				++aroundMinesCount;
			if (9 == allPosMineCount[i ][j + 1])
				++aroundMinesCount;
			if (9 == allPosMineCount[i + 1][j - 1])
				++aroundMinesCount;
			if (9 == allPosMineCount[i + 1][j])
				++aroundMinesCount;
			if (9 == allPosMineCount[i + 1][j + 1])
				++aroundMinesCount;

			allPosMineCount[i][j] = aroundMinesCount;
		}
	}

	return ;
}

void EventWorker::getNullAndAroundPos(std::map<std::pair<int, int>, QString >& result)
{

	std::stack<std::pair<int, int> > stack;
	std::pair<int, int> newPair;
	newPair.first = xPosition;
	newPair.second = yPosition;
	stack.push(newPair);
	getAllNullPos(stack, result);

	int x;
	int y;
	std::set<std::pair<int, int> > mineRound;
	for (auto nullPos : result)
	{
		x = nullPos.first.first;
		y = nullPos.first.second;

		int around[8][2] = { {x - 1,y - 1},{x - 1,y}, {x - 1,y + 1}, {x ,y - 1},
{x ,y + 1}, {x + 1,y - 1}, {x + 1,y}, {x + 1,y + 1} };
		for (int i = 0; i < 8; ++i)
		{
			if (0 == around[i][0] * around[i][1])
				continue;
			else if ((around[i][0] == xSize + 1) || (around[i][1] == ySize + 1))
				continue;
			else
			{
				std::pair<int, int> pair = { around[i][0] ,around[i][1] };
				if (result.find(pair) == result.end())
					mineRound.insert(pair);
			}
		}
	}
	int aroundMineCount;
	QString picPath;
	for (auto newPos : mineRound)
	{
		aroundMineCount = allPosMineCount[newPos.first][newPos.second];
		picPath = QString(":/MineSweeping/%1").arg(aroundMineCount);
		result[newPos] = picPath;
	}
}
void EventWorker::getAllNullPos(std::stack<std::pair<int,int> >& stack, std::map<std::pair<int, int>, QString >& result)
{
	std::pair<int, int> topPair = stack.top();
	result[topPair] = QString(":/MineSweeping/null");
	int x = topPair.first;
	int y = topPair.second;
	int around[8][2] = { {x - 1,y - 1},{x - 1,y}, {x - 1,y + 1}, {x ,y - 1},
	{x ,y + 1}, {x + 1,y - 1}, {x + 1,y}, {x + 1,y + 1} };
	for (int i = 0; i < 8; ++i)
	{
		std::pair<int, int> newPair;
		newPair.first = around[i][0];
		newPair.second = around[i][1];
		if ((0 == allPosMineCount[around[i][0]][around[i][1]])&&(result.find(newPair) == result.end()))
		{
			stack.push(newPair);
			getAllNullPos(stack, result);
		}
	}
	stack.pop();
}


void EventWorker::calculateMousePressResult()
{
	if (!isRunning)
		return;

	MousePressResultValue value;

	std::map<std::pair<int, int>, QString > result;
	if (0 == allPosMineCount[xPosition][yPosition])
	{
		getNullAndAroundPos(result);
		value.pressType = CLICK_NULL;
		value.result = result;

	}
	else if (9 == allPosMineCount[xPosition][yPosition])
	{
		value.pressType = CLICK_MINE;
		
	}
	else
	{
		value.pressType = CLICK_MINE_AROUND;
		value.aroundMineCount = allPosMineCount[xPosition][yPosition];
	}

	emit mousePressResult(QVariant::fromValue(value));
}