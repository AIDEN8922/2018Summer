#include "PaintArea.h"
#include "QDebug"

PaintArea::PaintArea()
{
	Image = QImage(700, 500, QImage::Format_RGB32); //������ʼ��
	backColor = qRgb(255, 255, 255);
	Image.fill(backColor);

	//������ʼ��
	isDrawing = false;
	angle = 0;
	shear = 0;
	scale = 1;
	curShape = None;
	isDrawing = false;
	penColor = Qt::black;              //���û��ʳ�ʼֵ
	penWidth = 5;
	penStyle = Qt::SolidLine;
}

PaintArea::~PaintArea()
{

}

//1.��ͼЧ������

void PaintArea::Rotate() //��ת
{
	angle += 45;
	update();
}

void PaintArea::Shear() //����
{
	shear = 0.2;
	update();
}

void PaintArea::Clear() //���
{
	Image.fill(backColor);
	update();
}

void PaintArea::zoomIn()//�Ŵ�
{
	scale *= 1.2;
	update();
}
void PaintArea::zoomOut()
{
	scale /= 1.2;
	update();
}

void PaintArea::zoomReturn()
{
	scale = 1;
	angle = 0;
	update();
}



void  PaintArea::setPenStyle(Qt::PenStyle style)                 //2.��������
{

	penStyle = style;
}

void  PaintArea::setPenWidth(int width)
{
	penWidth = width;
}

void  PaintArea::setPenColor(QColor color)
{
	penColor = color;
}

//void  PaintArea::setBrushColor(QColor color)       //2.0������
//{
//	brushColor = color;
//}



void PaintArea::setImageSize(int width, int height)
{
	QImage newImage(width, height, QImage::Format_RGB32);
	Image = newImage;
	update();
}

//void PaintArea::setImageColor(QColor color)
//{
//	backColor = color.rgb();
//	Image.fill(backColor);
//	update();
//}



QSize PaintArea::getImageSize()
{
	return Image.size();
}



bool PaintArea::saveImage(const QString& fileName, const char* fileFormat)//����ͼƬ
{
	QImage visibleImage = Image;
	if (visibleImage.save(fileName, fileFormat))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool PaintArea::openImage(const QString& fileName)
{
	QImage loadedImage;
	loadedImage.load(fileName);
	QSize newSize = loadedImage.size();

	setImageSize(newSize.width(), newSize.height());
	Image = loadedImage;
	update();
	return true;
}

/////////////////////////////////////////

void PaintArea::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		isDrawing = true;
		lastPoint = event->pos();
	}
}

void PaintArea::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		endPoint = event->pos();
		isDrawing = false;
		paint(Image);                                                            //��ԭͼ�ϵ����һ�λ�ͼ
	}
}

void PaintArea::mouseMoveEvent(QMouseEvent *event)                                    //Ӧ��˫����
{

	endPoint = event->pos();
	if (curShape == None || curShape == ERASER)
	{
		paint(Image);
		isDrawing = false;
	}
	else
	{
		temImage = Image;
		paint(temImage);
	}
}

void PaintArea::paintEvent(QPaintEvent *event)
{

	QPainter painter(this);

	painter.scale(scale, scale);
	painter.drawImage(0, 0, Image);

	if (isDrawing)
	{
		painter.drawImage(0, 0, temImage);
	}
	else
	{
		if (angle)
		{
			QImage copyImage = Image;
			QPainter p(&copyImage);
			QPointF center(copyImage.width() / 2.0, copyImage.height() / 2.0);

			//QPainter p(&Image);
			//QPointF center(Image.width() / 2.0, Image.height() / 2.0);         //����

			p.translate(center);
			p.rotate(angle);
			p.translate(-center);
			p.drawImage(0, 0, Image);                                      //�������⸴������ϵͳ���������
			Image = copyImage;
			angle = 0;

		}

		if (shear)
		{
			QImage copyImage = Image;
			QPainter p(&copyImage);
			p.shear(shear, shear);
			p.drawImage(0, 0, Image);
			Image = copyImage;
			shear = 0;
			//��bug
		}
	}

}

void PaintArea::setShape(ShapeType shape)                        //4.��������ͼ�� 3.��Ƥ
{
	curShape = shape;
}


void PaintArea::paint(QImage& TheImage)
{
	QPainter p(&TheImage);   //����ʱ��ͼ���ϻ�ͼ

	QPen pen;                                                      //����������brush 2.0
	pen.setColor(penColor);
	pen.setWidth(penWidth);
	pen.setStyle(penStyle);
	p.setPen(pen);

	QRect rect;

	int a, b, c, d;
	a = lastPoint.x() / scale;
	b = lastPoint.y() / scale;
	c = (endPoint.x() - a) / scale;
	d = (endPoint.y() - b) / scale;

	switch (curShape)
	{
	case None:    //����ѡ��
	{
		p.drawLine(lastPoint / scale, endPoint / scale);
		lastPoint = endPoint;
		break;
	}
	case Rectangle:
	{
		p.drawRect(a, b, c, d);
		break;
	}
	case Ellipse:
	{
		p.drawEllipse(a, b, c, d);
		break;
	}
	case ERASER:
	{
		rect = QRect(lastPoint, QPoint(lastPoint.x() + 10, lastPoint.y() + 10));
		p.eraseRect(rect);
		lastPoint = endPoint;
		break;
	}
	case SEED_FILL4:
	{
		QColor backColor = Image.pixelColor(lastPoint);
		int x = lastPoint.x();
		int y = lastPoint.y();
		seedFill4(x, y, penColor);
	}
	case FILL_SCAN:
	{
		QColor backColor = Image.pixelColor(lastPoint);
		points seed;
		seed.x = lastPoint.x();
		seed.y = lastPoint.y();
		FillScan(seed, penColor);
	}
	}
	update();

}


void PaintArea::seedFill4(int x, int y, QColor Fcolor)
{
	QColor currentColor = Image.pixelColor(x, y);
	if (currentColor == backColor)
	{
		Image.setPixelColor(x, y, Fcolor);
		seedFill4(x, y - 1, Fcolor);
		seedFill4(x, y + 1, Fcolor);
		seedFill4(x - 1, y, Fcolor);
		seedFill4(x + 1, y, Fcolor);
	}
}

void PaintArea::FillScan(points seed, QColor Fcolor)
{
	
	//���ջ�е����ӵ㲢Ѱ���µ����ӵ�

	SeedPoints_Stack.push(seed);                                       //��ʼ��Ϊ������㣬�õ����꣬ѹ��ջ
	while (!SeedPoints_Stack.isEmpty())                                //ջ�в�Ϊ�վ����
	{
		points seedXY;
		seedXY = SeedPoints_Stack.pop();

		int seedX = seedXY.x;                             //���ӵ��x,y����
		int seedY = seedXY.y;

		int xR = seedX;                                  //����������
		int xL = seedX;
		int y = seedY;                                   //�ڸ�������
		while (backColor == Image.pixelColor(xR, y))     //�������꣬�����ɫΪԭ����ɫ����ô�����
		{
			Image.setPixelColor(xR, y, Fcolor);
			xR++;
			c_area++;                                    //����ͳ�����
		}
		xR--;                                            //�ص�������
		xL--;                                            //��ʼ�����һ�㣬��Ϊ�ո�xL�Ѿ����Ϲ�ɫ��
		while (backColor == Image.pixelColor(xL, y))
		{
			Image.setPixelColor(xL, y, Fcolor);
			xL--;
			c_area++;                                    //����ͳ�����
		}
		xL++;                                            //�ص�������
	
		//����Ѱ�����ӵ�

		points ScanPoint;                                //�µ�ɨ��㣬Ϊ������ϵ�һ��
		ScanPoint.x = xL;
		ScanPoint.y = y + 1;
		//���㷨��˳������
	Judgement:
		switch (Image.pixelColor(ScanPoint.x, ScanPoint.y) == backColor)
		{
		case 1:                                              //�µĵ�����Ǳ���ɫ�����������ɨ��
			ScanPoint.x++;
			remark = 1;
			goto Judgement;
			break;
		case 0:                                               //�µĵ�����ǲ�Ϊ����ɫ��remark����==1���򽫸ĵ�ǰһ��ѹջ
			if (remark)
			{
				ScanPoint.x--;
				SeedPoints_Stack.push(ScanPoint);
				remark = 0;
				ScanPoint.x++;
			}

			if (ScanPoint.x <= xR - 2)                       //�ж��ǲ��������߽������ĺڵ�
			{
				ScanPoint.x++;
				goto Judgement;
			}
			break;
		}

		//����Ѱ�����ӵ㣬����ͬ��
		ScanPoint.x = xL;                                               //ɨ���Ϊ�·��ĵ�
		ScanPoint.y = y - 1;
	Judgement2:
		switch (Image.pixelColor(ScanPoint.x, ScanPoint.y) == backColor)
		{
		case 1:
			ScanPoint.x++;
			remark2 = 1;
			goto Judgement2;
			break;
		case 0:
			if (remark2)
			{
				ScanPoint.x--;
				SeedPoints_Stack.push(ScanPoint);
				remark2 = 0;
				ScanPoint.x++;
			}

			if (ScanPoint.x <= xR - 2)
			{
				ScanPoint.x++;
				goto Judgement2;
			}
			break;
		}
	}
}

int PaintArea::get_data()
{
	return c_area;
}

void PaintArea::height_width()
{
	int x, y;
	c_width = 0;
	bool remark=1;
	int width_x=0, height_y=0;
	
	for ( y = 0; y < 500; y++)  //�ӵ�һ���㿪ʼ�жϣ�ֱ���ҵ���һ���ڵ㣬���ҵ����һ���ڵ㣬��Ϊ�������ֵ�����������ֵ��Ϊ�ܵĿ��
	{
        for ( x = 0; x < 700; x++)
		{
			if ((Image.pixelColor(x, y) != backColor))
			{
				if (remark==1)
				{
					width_x = x;
					remark = 0;
				}
				if (c_width<x-width_x)
				{
					c_width = x - width_x;
				}
			}
		}
		x = 0;
		remark = 1;
	}
	c_width++;

	c_height = 0;
	remark = 1;
	width_x = 0, height_y = 0;   //�߶����ȵķ�����ͬ

	for (x = 0; x < 700; x++)
	{
		for (y = 0; y < 500; y++)
		{
			if ((Image.pixelColor(x, y) != backColor))
			{
				if (remark == 1)
				{
					c_height = y;
					remark = 0;
				}
				if (c_height<y - height_y)
				{
					c_height=y-height_y;
				}
			}
		}
		y = 0;
		remark = 1;
	}
	c_height++;
}