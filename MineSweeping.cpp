#include "MineSweeping.h"


MineSweeping::MineSweeping(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	setWindowIcon(QIcon(QString(":/MineSweeping/mine")));
	setWindowTitle(QString("扫雷"));
	connect(ui.okPB, &QPushButton::clicked, this, &MineSweeping::typesettingChangeSlot);

	timer.setSingleShot(true);
	connect(&timer, &QTimer::timeout, this, &MineSweeping::typesettingChangeSlot);
	connect(ui.widget, &CenterWidget::lastMineCountChange, this, &MineSweeping::lastMineCountChangeSlot);
	timer.start(500);
}

void MineSweeping::typesettingChangeSlot()
{
	int xSize = ui.XSB->value();
	int ySize = ui.YSB->value();
	ui.widget->setMainLayout(xSize, ySize);
	currentLastMineCount = (int)(xSize*ySize / 10);
	ui.lastMineCountLabel->setText(QString("剩余个数:<font color=\'red\'>%1").arg(currentLastMineCount));
	return;
}
void MineSweeping::lastMineCountChangeSlot(const int& i)
{
	currentLastMineCount += i;
	ui.lastMineCountLabel->setText(QString("剩余个数:<font color=\'red\'>%1").arg(currentLastMineCount));
}