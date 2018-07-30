#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_task2.h"
#include<qdebug.h>
#include<qevent.h>
#include<QMouseEvent>
#include<qdialog.h>
#include<qpainter.h>
#include<qpainterpath.h>
#include<qpoint.h>
#include<qpushbutton.h>
#include<vector>
#include<deque>
#include<qlayout.h>
#include<qtextbrowser.h>
#include<qimage.h>
#include<qmessagebox.h>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<qfiledialog.h>
extern int debug;
extern bool on_render;
struct Geometry {
	double h = 0, w = 0,s = 0;//��¼���Ჿ�ֵļ��β���
};

class myWidget :public QWidget {
	Q_OBJECT
private:
    //���ָ��û��ıʼǸ���paintpath���Ƴ�����paintpath����С��С�ε�ֱ���γ�
	//�û����myWidget��һ�㣬�õ������ͨ���źŲ۴���start,֮���ٲ��ϵ�MouseMove��
	//����������᲻�ϵĴ���endֵ��ÿ�δ���endֵ��ֻҪ��path��Lineto�����ӳ���end�������
	QPoint start = QPoint(0, 0), end = QPoint(0, 0); 
	QPen pen = QPen(Qt::black, 5);   
	QBrush brush = QBrush(Qt::red);
	QPainterPath current;
	QImage* img;    //�������ز�����ͼƬ
	QImage tmp;      //ֻ����render���õ��������˵��
	cv::Mat mat;     //���ڽ���opencv�ĺ����򿪺ͱ���ͼƬ��ֻ����QImage���û�ѡ��ͼƬ֮���һ���н飬��Ҫ�Ĳ�����QImage��
	std::vector<QPainterPath> allDraw;    //���ڱ������еıʼǣ����ÿ����̧��һ�ξ���һ�αʼ�
	std::deque<QPoint> pointsToSearch;     //�������������㷨�Ķ���

	Geometry seedSearchBrush(int x, int y,QColor boundColer, QColor brushColor);
	int saveOrDiscard();
	bool isModified=false;    //���ڼ�¼ͼƬ���Ƿ����޸ģ��Դ��ж��Ƿ����saveOrDiscard
public:
    myWidget(QWidget* parent=Q_NULLPTR)
		:QWidget(parent)
	{
	    img = new QImage(this->size(), QImage::Format_RGB888);
		setPalette(QPalette(Qt::white));
	}
	void on_draw_trigger();
	void on_brush_trigger();
	void on_open_trigger();
protected:
	//��myWidget���󴴽�����ʾ����ʱ��������Ϊ�ᵼ����Щ�¼����������ϵķ����ź�
	void mousePressEvent(QMouseEvent* e) {
		emit startDrawing(e->pos());
	}
	void mouseReleaseEvent(QMouseEvent* e) {
		emit stopDrawing();
	}
	void mouseMoveEvent(QMouseEvent* e) {
		emit onDrawing(e->pos());
	}
	void paintEvent(QPaintEvent* e) {
		//myWidget����ʾ����2������ɣ���һ������QImage����drawImage��������myWidget�ϵ�ͼ��
		//����Ҳ��������Ĳ��֣���Ϊ���������Image�Ͻ��е�)���ڶ�������֮����ȥ��ÿһ��QPaintPath,
		QPainter painter(this);
		painter.setPen(pen);
		painter.setBrush(Qt::NoBrush);
		qDebug() << "pAINT";
		if (on_render) { painter.drawImage(0, 0, tmp); }  
		else painter.drawImage(0,0,*img);            
		//ÿ���Զ����û���update()��������paintevent()����ʱ���ҵ�����ǣ���Ĭ��ֻ�ử��Widget�������ϸ���޹صĲ���
		//Ҳ������������ı߿�ʲô�ģ���������ʼ�������ϸ��Ҫ��painteventҲ֪��Ҫ��ôȥ��������Ҳ̫ţ���ˣ���������ȥ����Ҫ��ôȥ����
		//��˱�����ÿ���ػ�ʱ��֮ǰ�����бʼ����ػ棬��Ȼ֮ǰ�ıʼ�������ʾ��
		for (auto iter = allDraw.begin(); iter != allDraw.end(); iter++) {
			painter.drawPath(*iter);
		}
		painter.end();
	}
	signals:
		void startDrawing(QPoint);
	signals:
		void stopDrawing();
	signals:
		void onDrawing(QPoint);

};

class task2 : public QMainWindow
{
	Q_OBJECT
private:
	Ui::task2Class ui;
	myWidget wd;     //��������˻����Ĺ�����֮�⣬��myWidget��Ϊ��ʾͼƬ��centerWidget
public:
	task2(QWidget *parent = Q_NULLPTR)
		:QMainWindow(parent)
	{
		ui.setupUi(this);
	    //������3�����������������ֱ�ʵ�ִ�ͼƬ�����ʣ����Ṧ��
		QAction* open = new QAction(tr("open"), this);
		QAction* draw=new QAction(tr("&draw"),this);
		QAction* brush=new QAction(tr("&brush"),this);
		open->setShortcut(QKeySequence::Open);
		connect(open, &QAction::triggered, &wd, &myWidget::on_open_trigger);   
		connect(draw, &QAction::triggered, &wd, &myWidget::on_draw_trigger);
		connect(brush, &QAction::triggered, &wd, &myWidget::on_brush_trigger);
		ui.mainToolBar->addAction(open);
		ui.mainToolBar->addAction(draw);
		ui.mainToolBar->addAction(brush);
		
		setCentralWidget(&wd);
	}
};

class myDialog :public QDialog {
	Q_OBJECT
		
public:
	QPushButton *ok;
	myDialog(QWidget* parent = nullptr)
		:QDialog(parent)
	{
		ok = new QPushButton(tr("ok"), this);
		QObject::connect(ok, &QPushButton::clicked, this, &myDialog::close);
	}
	signals:
		void finished();
protected:
	void closeEvent(QCloseEvent* e) {
		qDebug() << "C";
		emit finished();
		QDialog::closeEvent(e);
	}
};
