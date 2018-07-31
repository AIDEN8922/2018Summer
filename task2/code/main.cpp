#include "task2.h"
#include <QtWidgets/QApplication>
#include<qlabel.h>
#include<qpixmap.h>
#include<qtimer.h>
int debug = 0;
bool on_render = 0;
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	task2 w;
	w.show();
	return a.exec();
}
void myWidget::on_draw_trigger() {
	
	qDebug() << "1";
	myDialog* dlg = new myDialog;
	dlg->setWindowFlag(Qt::WindowStaysOnTopHint);
	dlg->setWindowTitle(tr("finish?"));

	QPushButton* undo = new QPushButton(tr("undo"));
	connect(undo, &QPushButton::clicked, [this]() {
		if (!allDraw.empty()) {
			allDraw.pop_back();
			update();
		}
	});

	QHBoxLayout* layout = new QHBoxLayout(dlg);
	layout->addWidget(undo);
	layout->addWidget(dlg->ok);
    dlg->show();
	//��ʾ�Ի���������ťһ�������ڱ���������ϵ�OK,myDialog��close()�������źŲ����ӣ�
	//close()��������Լ����ص�closeevent()�������رմ��ڣ�������finish�ź�
	connect(dlg, &myDialog::finished, [this]() {
		//������һ������ȥ����finished�źţ��źŷ����ǣ�startdrawing������stopdrawing�������ڹ������press��release�ź�
		//Ҳ����˵�������OK�������ȥ������潫�����к���ȥ��Ӧ��
		qDebug() << "F";
		disconnect(this, &myWidget::startDrawing, 0, 0);
		disconnect(this, &myWidget::stopDrawing, 0, 0);
		isModified = true;
		//���ǵĻ�������QWidget�Ͻ��л��Ƶģ����û��ʻ���ʼ�֮�󣬱������ͬ����QImage�У�����brush������������
		//�����õ���render�������ǰ�*this����ʾ����img����N��ʧ�ܺ��ҷ���render�����*this�Ļ��ƺ���
		//������Ҳ���ǡ����ĺ��壬����Ҫ���Լ�����imgչ�֣����㻭��img��Ҳ�ǵ����Լ���paintEvent
		//������ô�����Ȱ�ԭ����img����������Ҳ��������tmpȥ��ʱ�����������ݵ�ԭ��
		on_render = 1;
		tmp = *img;
		this->render(img);
		on_render = 0;
	});

	connect(this, &myWidget::startDrawing, [this](QPoint start) {
		qDebug() << "p";
		current = QPainterPath(start);
		connect(this, &myWidget::onDrawing, [this](QPoint end) {
			qDebug() << "m";
			current.lineTo(end);
		});
	});
	connect(this, &myWidget::stopDrawing, [this]() {
		qDebug() << "R";
		disconnect(this, &myWidget::onDrawing, 0, 0);
		allDraw.push_back(current);
		update();
	});
}
void myWidget::on_brush_trigger() {
	myDialog* dlg = new myDialog;
	dlg->setWindowFlag(Qt::WindowStaysOnTopHint);
	QTextBrowser* tb = new  QTextBrowser(dlg);
	QHBoxLayout *layout=new QHBoxLayout(dlg);
	layout->addWidget(dlg->ok);
	layout->addWidget(tb);
	dlg->setWindowTitle(tr("finish?"));
	dlg->show();

	connect(dlg, &myDialog::finished, [this]() {
		qDebug() << "F";
		disconnect(this, &myWidget::startDrawing, 0, 0);
	});
	Geometry result;
	connect(this, &myWidget::startDrawing, [this,tb,&result](QPoint start) {
		isModified = true;
		result = seedSearchBrush(start.x(),start.y(),pen.color(),brush.color());
		QString str;
		str.sprintf("height: %f  width: %f  surface: %f", result.h, result.w, result.s);
		tb->setText(str);
		update();
	});
}
Geometry myWidget::seedSearchBrush(int x, int y, QColor boundColor,QColor brushColor) {
	QPoint cur;
	double pixelAmt = 0, maxH = 0, maxW = 0, minH = 10000, minW = 10000;
	int wLimit = this->width(), hLimit = this->height();
	if (img->pixelColor(x, y) != boundColor&&img->pixelColor(x,y)!=brushColor) {
		img->setPixelColor(x, y, brushColor);
		pointsToSearch.push_back(QPoint(x, y));
		pixelAmt++;
	}
	while (!pointsToSearch.empty()) {
		cur = pointsToSearch.front();
		pointsToSearch.pop_front();

		if (cur.x() > maxW) maxW = cur.x();
		if (cur.y() > maxH) maxH = cur.y();
		if (cur.x() < minW) minW = cur.x();
		if (cur.y() < minH) minH = cur.y();
		
		if (cur.x() + 1 <= wLimit && img->pixelColor(cur.x()+1, cur.y()) != brushColor && img->pixelColor(cur.x() + 1, cur.y()) != boundColor) {
			img->setPixelColor(cur.x()+1,cur.y(), brushColor);
			pointsToSearch.push_back(QPoint(cur.x() + 1, cur.y()));
			pixelAmt++;
		}
		if (cur.x() - 1 >= 0 && img->pixelColor(cur.x() - 1, cur.y()) != boundColor && img->pixelColor(cur.x() - 1, cur.y()) != brushColor) {
			img->setPixelColor(cur.x() - 1, cur.y(), brushColor);
			pointsToSearch.push_back(QPoint(cur.x() -1, cur.y()));
			pixelAmt++;
		}
		if (cur.y() + 1 <= hLimit && img->pixelColor(cur.x() , cur.y()+1) != boundColor && img->pixelColor(cur.x(), cur.y() + 1) != brushColor) {
			img->setPixelColor(cur.x(), cur.y()+1, brushColor);
			pointsToSearch.push_back(QPoint(cur.x(), cur.y()+1));
			pixelAmt++;
		}
		if (cur.y() -1 >= 0 && img->pixelColor(cur.x(), cur.y() - 1) != boundColor && img->pixelColor(cur.x(), cur.y() - 1) != brushColor) {
			img->setPixelColor(cur.x(), cur.y() - 1, brushColor);
			pointsToSearch.push_back(QPoint(cur.x(), cur.y() - 1));
			pixelAmt++;
		}
	}
	long dencePerCM = img->dotsPerMeterX() * img->dotsPerMeterY()/10000;
	qDebug() << (maxH - minH) / (img->dotsPerMeterY() / 100) << " " << (maxW - minW) / (img->dotsPerMeterX() / 100) << " " << pixelAmt / dencePerCM;
	return Geometry{ (maxH-minH)/(img->dotsPerMeterY()/100),(maxW-minW)/(img->dotsPerMeterX() / 100),pixelAmt / dencePerCM };
}

int myWidget::saveOrDiscard() {
	QMessageBox msg;
	msg.setText("save the picture?");
	msg.setStandardButtons(QMessageBox::Save
		| QMessageBox::Discard
		| QMessageBox::Cancel);
	msg.setDefaultButton(QMessageBox::Save);
	int selt = msg.exec();
	QString savePath; std::string;
	std::string fileAsSave;
	//QScreen *screen = QGuiApplication::primaryScreen();
	switch (selt) {
	case QMessageBox::Save:
		savePath = QFileDialog::getSaveFileName(this, tr("Save Image"), "", tr("Images (*.png *.bmp *.jpg)"));
		fileAsSave = savePath.toStdString();
		cv::cvtColor(mat, mat, CV_BGR2RGB);
		cv::imwrite(fileAsSave, mat);
		return QMessageBox::Save;
		break;
	case QMessageBox::Discard:
		return QMessageBox::Discard;
		break;
	case QMessageBox::Cancel:
		return QMessageBox::Cancel;
		break;
	}
}
void myWidget::on_open_trigger() {
	qDebug() << "open";
	int selt;
	if (isModified) {
		selt = saveOrDiscard();
		if (selt == QMessageBox::Cancel) return;
	}
	//����һ��ѡ����ļ��ĶԻ��򣬸�������������򴰿ڣ������Ǵ�ͼƬ��Ĭ��·��Ϊ����㣬�ļ����͹���Ϊ�����ļ�
	QFileDialog* seltOpenFile = new QFileDialog(this, tr("��ͼƬ"), ".", tr("Image File(*.jpg *.png *.bmp)"));
	label: QString path = seltOpenFile->getOpenFileName();
	mat = cv::imread(path.toStdString());  //��ʼ��ͼƬ����

	if (mat.channels() == 3) cv::cvtColor(mat, mat, CV_BGR2RGB);
	else {
		if(QMessageBox::warning(this, tr("Unsupported Image Type"), tr("try again?"),QMessageBox::Ok|QMessageBox::Cancel)==QMessageBox::Ok)
			goto label;
		else return;
	}

	//��MAT��ʽת��ΪQImage��ʽ��������С����Ϊ��myWidget��Сһ��
	*img= (QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888)).scaled(this->size(), Qt::KeepAspectRatio);  //��ͼƬ����������Ϊ�Ӵ���С

	allDraw.clear();
	update();
}
