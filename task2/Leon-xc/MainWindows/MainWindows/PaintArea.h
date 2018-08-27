#pragma once
#include "qwidget.h"
#include "QWidget"
#include "QMouseEvent"
#include "QPoint"
#include "QPaintEvent"
#include "QPainter"
#include "QPen"
#include "QStack"


class PaintArea :
	public QWidget
{
public:
	PaintArea();
	~PaintArea();



	//����ջ
	struct points
	{
		int x;
		int y;
	};

	QStack<points> SeedPoints_Stack;

	//���û�������
	void setImageSize(int width, int height);
	void setImageColor(QColor color);


	bool saveImage(const QString& fileName, const char* fileFormat);//����ͼƬ
	bool openImage(const QString& fileName);
	QSize getImageSize();

	//���ֹ��ܺ������������ʷ�� ��ɫ����ȣ������ɫ                                 ??

	void setPenStyle(Qt::PenStyle style);
	void setPenWidth(int width);
	void setPenColor(QColor color);
	void seedFill4(int x, int y, QColor Fcolor);
	void FillScan(points seed, QColor Fcolor);

	//����ͼ�β˵�����������
	enum ShapeType  //ö�ٱ���������ͼ�ε�ѡ��
	{
		None,
		Rectangle,
		Ellipse,
		ERASER,
		SEED_FILL4,
		FILL_SCAN
	};
	ShapeType curShape;

	//��ת�����ţ�����
	void zoomIn(); //�Ŵ�
	void zoomOut();
	void zoomReturn();

	void Rotate(); //��ת
	void Shear();  //����
	void Clear();  //���
	void setShape(ShapeType shape);  //����Ҫ���Ƶ�ͼ��


private:

	//���ֲ���

	QImage Image;
	QImage temImage;
	QColor backColor;          //�������

	int angle;
	qreal scale;
	qreal shear;              //��ת����Ŵ�Ȳ���

	QColor penColor;
	int penWidth;
	Qt::PenStyle penStyle;

	QPoint lastPoint, endPoint;

	bool isDrawing;

	bool remark;
	bool remark2;
	//��������жϴӰױ䵽�˺ڡ�

	//����¼���滭�¼�
	void paintEvent(QPaintEvent *event);//�ػ��¼�
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent* event);
	void paint(QImage& TheImage);

public:
	//�õ�����
	int get_data();
	void height_width();

	//���ݲ���
	int c_height;
	int c_width;
	int c_area = 0;
};


