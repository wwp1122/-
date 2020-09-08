#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MineSweeping.h"

#include <set>
#include <QTimer>

#include "CenterWidget.h"

class MineSweeping : public QMainWindow
{
	Q_OBJECT

public:
	MineSweeping(QWidget *parent = Q_NULLPTR);

private:
	Ui::MineSweepingClass ui;
	QTimer timer;
	int currentLastMineCount;

private slots:
	void typesettingChangeSlot();
	void lastMineCountChangeSlot(const int& i);
};
