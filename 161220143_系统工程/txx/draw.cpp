#include "draw.h"


Draw::Draw(QWidget *parent)
    : QOpenGLWidget(parent)
{
    currentColor[0]=0;
    currentColor[1]=0;
    currentColor[2]=0;
    currentMode=0;
    curPicID=0;
    penSize=1;
    isChoosingPoints=false;
    PolyDrawn=false;
    setMouseTracking(false);
    //img.resize(width(),height());

}

Draw::~Draw()
{
}

void Draw::setColor(int r,int g,int b)
{
    currentColor[0]=r;
    currentColor[1]=g;
    currentColor[2]=b;
    update();
}
void Draw::setMode(int i)
{
    currentMode=i;
    //QMessageBox::about(NULL, "About", QString::number(i));
    //QMessageBox::about(NULL, "About", QString::number(penSize));

}
void Draw::setpenSize(int i)
{
    penSize=i;
}



int Draw::C(int n,int k)
{
    if(n==k||k==0)
        return 1;
    else
        return C(n-1,k-1)+C(n-1,k);
}

void Draw::drawPoint(double x,double y)
{
    onePoint *p=new onePoint;
    p->x=x;
    p->y=y;
    p->color[0]=currentColor[0];
    p->color[1]=currentColor[1];
    p->color[2]=currentColor[2];
    p->size=penSize;
    p->pid=curPicID;
    p->isChosen=false;
    allPoints.push_back(*p);
    //QMessageBox::about(NULL, "About", QString::number(x)+" "+QString::number(y));
}

void Draw::drawLine(double x1, double y1, double x2, double y2)
{
    //QMessageBox::about(NULL,"About",QString::number(allPoints.size()));
    //QMessageBox::about(NULL, "About", QString::number(allPoints[1].x)+" "+QString::number(allPoints[1].y));
    //QMessageBox::about(NULL,"About",QString::number(curPicID));
    //QMessageBox::about(NULL,"About",QString::number(allPoints[0].size));
    double dx=x2-x1,dy=y2-y1;
    if(dx==0)//垂直
    {
        double x=x1;
        double y=(y1<y2)?y1:y2;
        dy=fabs(y2-y1);
        while(dy>=0)
        {
            drawPoint(x,y);
            y++;
            dy--;
        }
    }
    else//斜率存在
    {
        //dx=abs(dx);
        //dy=abs(dy);
        double steps=(abs(dy)>abs(dx))?abs(dy):abs(dx);
        double delta_x=dx/steps,delta_y=dy/steps;
        double x=x1,y=y1;
        drawPoint(x,y);
        for(int i=1;i<steps;i++)
        {
            x+=delta_x;
            y+=delta_y;
            drawPoint(x,y);
        }
    }
}

void Draw::drawCurve()
{
    for(double t=0;t<=1.0;t+=0.001)
    {
        double x=0,y=0;
        double s=1.0-t;
        for(int i=0;i<4;i++)
        {
            x+=drawingPoints[i].first*pow(s,3-i)*pow(t,i)*C(3,i);
            y+=drawingPoints[i].second*pow(s,3-i)*pow(t,i)*C(3,i);
        }
        drawPoint(x,y);
    }
}

void Draw::drawRect(double x1, double y1, double x2, double y2)
{
    drawLine(x1, y1, x1, y2);
    drawLine(x1, y1, x2, y1);
    drawLine(x1, y2, x2, y2);
    drawLine(x2, y1, x2, y2);
}

void Draw::drawFilledRect(double x1, double y1, double x2, double y2)
{
    double x,X,y,Y;
    x=(x1<x2)?x1:x2;
    X=(x1>x2)?x1:x2;
    y=(y1<y2)?y1:y2;
    Y=(y1>y2)?y1:y2;
    for(double i=x;i<=X;i++)
        drawLine(i,y,i,Y);
}

void Draw::drawPoly()
{
    int temp=drawingPoints.size();
    for(int i=0;i<temp-1;i++)
        drawLine(drawingPoints[i].first,drawingPoints[i].second,drawingPoints[i+1].first,drawingPoints[i+1].second);
    drawLine(drawingPoints[0].first,drawingPoints[0].second,drawingPoints[temp-1].first,drawingPoints[temp-1].second);
}

void Draw::drawFilledPoly()
{
    Edge *AET;//活性表
    Edge **ET=new Edge*[height()];
    int maxY=-1;
    for(unsigned i=0;i<drawingPoints.size();++i)
    {
         if(drawingPoints[i].second>maxY)
            maxY = drawingPoints[i].second; //找到最高点
    }
    for (int i=0;i<maxY;i++)//初始化ET表
    {
        ET[i]=new Edge;
        ET[i]->next=NULL;
    }
    AET=new Edge;
    AET->next=NULL;//AET初始化
    for (unsigned i=0; i<drawingPoints.size(); i++) //i和i+1的边写入边表
    {
        int x1=drawingPoints[i].first;//点1的横坐标
        int x2=drawingPoints[(i+1)%drawingPoints.size()].first;//点2的横坐标
        int y0=drawingPoints[(i-1+drawingPoints.size()) % drawingPoints.size()].second;//点1另一个边另一个点的纵坐标
        int y1=drawingPoints[i].second;//点1的纵坐标
        int y2=drawingPoints[(i+1)%drawingPoints.size()].second;//点2的纵坐标
        int y3=drawingPoints[(i+2)%drawingPoints.size()].second;//点2另一个边另一个点的纵坐标
        if(y1 == y2)//斜率为0，不用管
            continue;
        int ymin=(y1>y2)?y2:y1;
        int ymax=(y1>y2)?y1:y2;
        double x=(y1>y2)?x2:x1;
        double dx=double((x1-x2))/double((y1-y2));
        if(((y1<y2)&&(y1>y0))||((y2<y1)&&(y2>y3)))//有一点是左顶点/右顶点，下端上移一格，
        {
            ymin++;
            x+=dx;
        }
        Edge *p=new Edge;
        p->Ymax=ymax;
        p->X=x;
        p->delta_x=dx;
        p->next=ET[ymin]->next;
        ET[ymin]->next=p;//插入对应ET表位置
    }
    for(int i=0;i<maxY;i++)//各扫描线i
    {
        while(ET[i]->next)//这一行ET不为空,把ET[i]的节点都插入AET
        {
            Edge *pInsert=ET[i]->next;//待插入边
            Edge *p=AET;
            while (p->next)
            {

                while((pInsert->X>p->next->X)//待插入的X大于目前位置X
                      ||(pInsert->X==p->next->X && pInsert->delta_x>p->next->delta_x))//目前一样，但增量更大
                {
                    p=p->next;
                    if(!p->next)
                        break;
                }
                break;//找到位置，
            }//找到AET中的插入位置，按X递增
            ET[i]->next=pInsert->next;
            pInsert->next=p->next;
            p->next=pInsert;
       }
       Edge *p=AET;
       while (p->next && p->next->next)//12之间，34之间....
       {
           for (int x=p->next->X;x<p->next->next->X;x++)
               drawPoint(x,i);
           p=p->next->next;
       }
       p=AET;
       while (p->next)
       {
           if (p->next->Ymax==i)//删去所有到头的边
           {
               Edge *temp=p->next;
               p->next=temp->next;
               temp->next=NULL;
               delete temp;
           }
           else//如果下个扫描线还能继续，+delta_x
           {
               p->next->X+=p->next->delta_x;
               p=p->next;
           }
       }
    }
}

void Draw::drawCircle8(double x,double y,double delta_x,double delta_y)
{
    drawPoint(x+delta_x,y+delta_y);
    drawPoint(x-delta_x,y+delta_y);
    drawPoint(x+delta_x,y-delta_y);
    drawPoint(x-delta_x,y-delta_y);
    drawPoint(x+delta_y,y+delta_x);
    drawPoint(x-delta_y,y+delta_x);
    drawPoint(x+delta_y,y-delta_x);
    drawPoint(x-delta_y,y-delta_x);
}

void Draw::drawCircle(double x,double y,double r)
{
    int delta_x=0;
    int delta_y=r;
    int d=3-2*r;
    drawCircle8(x,y,delta_x,delta_y);
    while (delta_x<delta_y)
    {
        if (d<0)
            d=d+4*delta_x+6;
        else
        {
            d=d+4*(delta_x-delta_y)+10;
            delta_y--;
        }
        delta_x++;
        drawCircle8(x,y,delta_x,delta_y);
    }
}

void Draw::drawFilledCircle(double x,double y,double r)
{
    int delta_x=0;
    int delta_y=r;
    int d=3-2*r;
    drawCircle8(x,y,delta_x,delta_y);
    while (delta_x<delta_y)
    {
        for(int temp=delta_x;temp<=delta_y;temp++)
            drawCircle8(x,y,delta_x,temp);
        if (d<0)
            d=d+4*delta_x+6;
        else
        {
            d=d+4*(delta_x-delta_y)+10;
            delta_y--;
        }
        delta_x++;
        //drawCircle8(x,y,delta_x,delta_y);
    }
}

void Draw::drawOval4(double x,double y,double delta_x,double delta_y)
{
    drawPoint(x+delta_x,y+delta_y);
    drawPoint(x-delta_x,y+delta_y);
    drawPoint(x+delta_x,y-delta_y);
    drawPoint(x-delta_x,y-delta_y);
}

void Draw::drawOval(double x,double y,double a,double b)
{
    int sqa=a*a;
    int sqb=b*b;
    int delta_x=0;
    int delta_y=b;
    int d=2*sqb-2*b*sqa+sqa;
    drawOval4(x,y,delta_x,delta_y);
    int P_x=round((double)sqa/sqrt((double)(sqa+sqb)));
    while(delta_x<=P_x)
    {
        if(d < 0)
            d+=2*sqb*(2*delta_x+3);
        else
        {
            d+=2*sqb*(2*delta_x+3)-4*sqa*(delta_y-1);
            delta_y--;
        }
        delta_x++;
        drawOval4(x,y,delta_x,delta_y);
    }
    d=sqb*(delta_x*delta_x+delta_x)+sqa*(delta_y*delta_y-delta_y)-sqa*sqb;
    while(delta_y>=0)
    {
        drawOval4(x,y,delta_x,delta_y);
        delta_y--;
    if(d<0)
    {
        delta_x++;
        d=d-2*sqa*delta_y-sqa+2*sqb*delta_x+2*sqb;
    }
    else
        d=d-2*sqa*delta_y-sqa;
    }
}

void Draw::drawFilledOval(double x,double y,double a,double b)
{
    int sqa=a*a;
    int sqb=b*b;
    int delta_x=0;
    int delta_y=b;
    int d=2*sqb-2*b*sqa+sqa;
    drawLine(x+delta_x,y+delta_y,x+delta_x,y-delta_y);
    drawLine(x-delta_x,y+delta_y,x-delta_x,y-delta_y);
    int P_x = round((double)sqa/sqrt((double)(sqa+sqb)));
    while(delta_x <= P_x)
    {
        if(d < 0)
            d+=2*sqb*(2*delta_x+3);
        else
        {
            d+=2*sqb*(2*delta_x+3)-4*sqa*(delta_y-1);
            delta_y--;
        }
        delta_x++;
        drawLine(x+delta_x,y+delta_y,x+delta_x,y-delta_y);
        drawLine(x-delta_x,y+delta_y,x-delta_x,y-delta_y);
    }
    d = sqb * (delta_x * delta_x + delta_x) + sqa * (delta_y * delta_y - delta_y) - sqa * sqb;
    while(delta_y >= 0)
    {
        drawLine(x+delta_x,y+delta_y,x+delta_x,y-delta_y);
        drawLine(x-delta_x,y+delta_y,x-delta_x,y-delta_y);
        delta_y--;
        if(d < 0)
        {
            delta_x++;
            d = d - 2 * sqa * delta_y - sqa + 2 * sqb * delta_x + 2 * sqb;
        }
        else
            d = d - 2 * sqa * delta_y - sqa;
    }
}

void Draw::choose(double x,double y)
{
    int choose_id=-1;
    for(int i=0;i<allPoints.size();i++)
    {
        if(abs(allPoints[i].x-x)<=1&&abs(allPoints[i].y-y)<=1)
        {
            choose_id=allPoints[i].pid;
            break;
        }
    }//找到所点的点的图形
    if(choose_id==-1)
        return;
    clean_chosenPoints();
    for(int i=0;i<allPoints.size();i++)
    {
        if(allPoints[i].pid==choose_id)
        {
            allPoints[i].isChosen=true;
            onePoint *p=new onePoint;
            *p=allPoints[i];
            chosenPoints.push_back(*p);
        }
    }
    isChoosingPoints=true;
}

void Draw::chooseRect(double x1, double y1, double x2, double y2)
{
    choose_cancel();
    for(int i=0;i<allPoints.size();i++)
    {
        double x=allPoints[i].x,y=allPoints[i].y;
        if((x-x1)*(x-x2)<0 && (y-y1)*(y-y2)<0)
        {
            allPoints[i].isChosen=true;
            onePoint *p=new onePoint;
            *p=allPoints[i];
            chosenPoints.push_back(*p);
        }
    }
    isChoosingPoints=true;
}

void Draw::choose_cancel()
{
    for(int i=0;i<allPoints.size();i++)
        allPoints[i].isChosen=false;
    clean_chosenPoints();
    isChoosingPoints=false;
    update();
}

void Draw::move(double delta_x, double delta_y)
{
    if(!isChoosingPoints)
        return;
    for(int i=0;i<allPoints.size();i++)
    {
        if(allPoints[i].isChosen)
        {
            allPoints[i].x+=delta_x;
            allPoints[i].y+=delta_y;
        }
    }
    update();
}

void Draw::rotate(double angle)
{
    if(!isChoosingPoints)
        return;
    angle=-angle/180*3.14159;
    double cosA=cos(angle),sinA=sin(angle);
    double x_min=width(),x_max=0,y_min=height(),y_max=0;
    for(int i=0;i<chosenPoints.size();i++)
    {
        if(chosenPoints[i].x<x_min)
            x_min=chosenPoints[i].x;
        if(chosenPoints[i].x>x_max)
            x_max=chosenPoints[i].x;
        if(chosenPoints[i].y<y_min)
            y_min=chosenPoints[i].y;
        if(chosenPoints[i].y>y_max)
            y_max=chosenPoints[i].y;
    }
    double center_x=(x_min+x_max)/2,center_y=(y_min+y_max)/2;
    for(int i=0;i<allPoints.size();i++)
    {
        if(allPoints[i].isChosen)
        {
            double x=allPoints[i].x,y=allPoints[i].y;
            double result_x=(x-center_x)*cosA-(y-center_y)*sinA;
            double result_y=(x-center_x)*sinA+(y-center_y)*cosA;
            allPoints[i].x=center_x+result_x;
            allPoints[i].y=center_y+result_y;
        }
    }
    update();
}

void Draw::zoom(double times)
{
    if(!isChoosingPoints)
        return;
    double x_min=width(),x_max=0,y_min=height(),y_max=0;
    for(int i=0;i<chosenPoints.size();i++)
    {
        if(chosenPoints[i].x<x_min)
            x_min=chosenPoints[i].x;
        if(chosenPoints[i].x>x_max)
            x_max=chosenPoints[i].x;
        if(chosenPoints[i].y<y_min)
            y_min=chosenPoints[i].y;
        if(chosenPoints[i].y>y_max)
            y_max=chosenPoints[i].y;
    }
    double center_x=(x_min+x_max)/2,center_y=(y_min+y_max)/2;
    for(int i=0;i<allPoints.size();i++)
    {
        if(allPoints[i].isChosen)
        {
            double x=allPoints[i].x,y=allPoints[i].y;
            double result_x=(x-center_x)*times;
            double result_y=(y-center_y)*times;
            allPoints[i].x=center_x+result_x;
            allPoints[i].y=center_y+result_y;
        }
    }
    update();
}

void Draw::changeColor()
{
    if(!isChoosingPoints)
        return;
    for(int i=0;i<allPoints.size();i++)
    {
        if(allPoints[i].isChosen)
        {
            allPoints[i].color[0]=currentColor[0];
            allPoints[i].color[1]=currentColor[1];
            allPoints[i].color[2]=currentColor[2];
        }
    }
    update();
}

void Draw::changeSize()
{
    if(!isChoosingPoints)
        return;
    for(int i=0;i<allPoints.size();i++)
    {
        if(allPoints[i].isChosen)
            allPoints[i].size=penSize;
    }
    update();
}

void Draw::openFile(QString filepath)
{
    newFile();
    image->load(filepath);
    int W=image->width(),H=image->height();
    for(int i=0;i<W;i++)
    {
        for(int j=0;j<H;j++)
        {
            QColor c=image->pixelColor(i,j);
            if(c.red()==c.green()&&c.red()==c.blue()&&c.green()==c.blue()&&c.red()>=200)
                continue;
            onePoint *p=new onePoint;
            p->color[0]=c.red();
            p->color[1]=c.green();
            p->color[2]=c.blue();
            p->pid=1;
            p->size=1;
            p->isChosen=false;
            p->x=i;
            p->y=height()-j;
            allPoints.push_back(*p);
        }
    }
    curPicID=2;
    update();
}

void Draw::saveFile(QString filepath)
{
    QPainter p;
    p.begin(image);
    image->fill(QColor(Qt::white).rgb());
    for(int i=0;i<allPoints.size();i++)
    {
        QPen pen;
        pen.setWidth(allPoints[i].size);
        pen.setColor(QColor(allPoints[i].color[0],allPoints[i].color[1],allPoints[i].color[2]));
        p.setPen(pen);
        p.drawPoint(allPoints[i].x,height()-allPoints[i].y);
    }
    p.end();
    image->save(filepath);
}

void Draw::newFile()
{
    clean_allPoints();
    clean_chosenPoints();
    clean_drawingPoints();
    clean_trashPoints();

    currentColor[0]=0;
    currentColor[1]=0;
    currentColor[2]=0;
    currentMode=0;
    curPicID=0;
    penSize=1;
    isChoosingPoints=false;
    PolyDrawn=false;
    setMouseTracking(false);
    update();
}

void Draw::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(1.0,1.0,1.0,1.0);
    glPointSize(penSize);
    glColor3i(currentColor[0], currentColor[1], currentColor[2]);
}

void Draw::resizeGL(int width,int height)
{
    glViewport(0, 0, width, height);//重置当前的视口（Viewport）
    glMatrixMode(GL_PROJECTION);//对投影矩阵应用随后的矩阵操作
    glLoadIdentity();//重置投影矩阵
    gluOrtho2D(0,this->size().width(),0,this->size().height());//按正交投影截取要显示的部分
    glMatrixMode(GL_MODELVIEW);//选择模型观察矩阵
    glLoadIdentity();
}

void Draw::paintGL()
{
    for(std::vector<onePoint>::iterator ite=allPoints.begin();ite!=allPoints.end();ite++)
    {
        if((*ite).isChosen==false)
            glColor3d(double((*ite).color[0])/255,double((*ite).color[1])/255,double((*ite).color[2])/255);
        else
            glColor3d(0,0,0);//已选中的设为黑色
        glPointSize((*ite).size);
        glBegin(GL_POINTS);
        glVertex3d((*ite).x,(*ite).y,0);
        glEnd();
        //qDebug()<<QString::number((*ite).x)+" "+QString::number((*ite).y)<<" r"<<(*ite).color[0]<<" g"<<(*ite).color[1]<<" b"<<(*ite).color[2];

    }
    glColor3i(currentColor[0], currentColor[1], currentColor[2]);
    glPointSize(penSize);
}

void Draw::Undo()
{
    if(allPoints.size()==0) return;
    curPicID--;
    int index=allPoints.size()-1;
    int index2=trashPoints.size()-1;
    while(trashPoints.size()&&trashPoints[index2].pid==curPicID)
    {
        trashPoints.pop_back();
        index2--;
    }
    while(allPoints.size()&&allPoints[index].pid==curPicID)
    {
        onePoint* p=new onePoint;
        *p=allPoints[index];
        trashPoints.push_back(*p);
        allPoints.pop_back();
        index--;
    }
    update();
}

void Draw::Redo()
{
    if(trashPoints.size()==0) return;
    int index=trashPoints.size()-1;
    int selectID=trashPoints[index].pid;
    while(trashPoints.size()&&trashPoints[index].pid==selectID)
    {
        onePoint* p=new onePoint;
        *p=trashPoints[index];
        p->pid=curPicID;
        allPoints.push_back(*p);
        trashPoints.pop_back();
        index--;
    }
    curPicID++;
    update();
}

void Draw::mousePressEvent(QMouseEvent *event)
{
    from_x=event->localPos().x();
    from_y=this->height()-event->localPos().y();
    double x=from_x,y=from_y;
    if(event->button()==Qt::LeftButton)
        LeftButtonPressed=true;
    else if(event->button()==Qt::RightButton)
        RightButtonPressed=true;
    if(currentMode==_POINT_)
    {
        if(event->button()==Qt::LeftButton)
            drawPoint(x,y);
    }
    else if(currentMode==_LINE_)
    {
        if(event->button()==Qt::LeftButton)
        {
            std::pair<double,double> *p=new std::pair<double,double>;
            p->first=from_x;
            p->second=from_y;
            if(drawingPoints.size()==0)//起始点
                drawingPoints.push_back(*p);
            else if(drawingPoints.size()==1)//终点
            {
                drawingPoints.push_back(*p);
                drawLine(drawingPoints[0].first,drawingPoints[0].second,drawingPoints[1].first,drawingPoints[1].second);
                curPicID++;
            }
            else if(drawingPoints.size()==2)//都画好了
            {
                clean_drawingPoints();
                drawingPoints.push_back(*p);
            }
        }
    }
    else if(currentMode==_CURVE_)
    {
        if(event->button() == Qt::LeftButton)
        {
            std::pair<double,double> *p=new std::pair<double,double>;
            p->first=from_x;
            p->second=from_y;
            if(drawingPoints.size()<3)//已有0/1/2个点，继续加
                drawingPoints.push_back(*p);
            else if(drawingPoints.size()==3)//目前加的是最后一个点
            {
                drawingPoints.push_back(*p);
                drawCurve();
                curPicID++;
            }
            else if(drawingPoints.size()==4)//新的
            {
                /*qDebug()<<drawingPoints[0].first<<" "<<drawingPoints[0].second<<endl;
                qDebug()<<drawingPoints[1].first<<" "<<drawingPoints[1].second<<endl;
                qDebug()<<drawingPoints[2].first<<" "<<drawingPoints[2].second<<endl;
                qDebug()<<drawingPoints[3].first<<" "<<drawingPoints[3].second<<endl;*/
                clean_drawingPoints();
                drawingPoints.push_back(*p);
            }
        }
    }
    else if(currentMode==_RECT_||currentMode==_FILLEDRECT_||currentMode==_CHOOSERECT_)
    {
        if(event->button() == Qt::LeftButton)
        {
            std::pair<double,double> *p=new std::pair<double,double>;
            p->first=from_x;
            p->second=from_y;
            if(drawingPoints.size()==0)//第一个点
                drawingPoints.push_back(*p);
            else if(drawingPoints.size()==1)//正在选第二个点
            {
                drawingPoints.push_back(*p);
                if(currentMode==_RECT_)
                    drawRect(drawingPoints[0].first,drawingPoints[0].second,drawingPoints[1].first,drawingPoints[1].second);
                else if(currentMode==_FILLEDRECT_)
                    drawFilledRect(drawingPoints[0].first,drawingPoints[0].second,drawingPoints[1].first,drawingPoints[1].second);
                else
                    chooseRect(drawingPoints[0].first,drawingPoints[0].second,drawingPoints[1].first,drawingPoints[1].second);
                curPicID++;
            }
            else if(drawingPoints.size()==2)
            {
                clean_drawingPoints();
                drawingPoints.push_back(*p);
            }
        }
    }
    else if(currentMode==_POLY_||currentMode==_FILLEDPOLY_)
    {
        std::pair<double,double> *p=new std::pair<double,double>;
        p->first=from_x;
        p->second=from_y;
        if(event->button() == Qt::LeftButton)
        {
            if(PolyDrawn)//之前的多边形已经画好，这是新的了
            {
                clean_drawingPoints();
                drawingPoints.push_back(*p);
                PolyDrawn=false;
            }
            else//仍在选取多边形的顶点
                drawingPoints.push_back(*p);
        }
    }
    else if(currentMode==_CIRCLE_||currentMode==_FILLEDCIRCLE_)
    {
        std::pair<double,double> *p=new std::pair<double,double>;
        p->first=from_x;
        p->second=from_y;
        if(event->button() == Qt::LeftButton)
        {
            if(drawingPoints.size()==0)//取圆心
                drawingPoints.push_back(*p);
            else if(drawingPoints.size()==1)//取圆上一点
            {
                drawingPoints.push_back(*p);
                double r=sqrt((drawingPoints[0].first-drawingPoints[1].first)*(drawingPoints[0].first-drawingPoints[1].first)+(drawingPoints[0].second-drawingPoints[1].second)*(drawingPoints[0].second-drawingPoints[1].second));
                if(currentMode==_CIRCLE_)
                    drawCircle(drawingPoints[0].first,drawingPoints[0].second,r);
                else
                    drawFilledCircle(drawingPoints[0].first,drawingPoints[0].second,r);
                curPicID++;
            }
            else if(drawingPoints.size()==2)//新的圆
            {
                clean_drawingPoints();
                drawingPoints.push_back(*p);
            }
        }
    }
    else if(currentMode==_OVAL_||currentMode==_FILLEDOVAL_)
    {
        std::pair<double,double> *p=new std::pair<double,double>;
        p->first=from_x;
        p->second=from_y;
        if(event->button() == Qt::LeftButton)
        {
            if(drawingPoints.size()==0)//第一个点
                drawingPoints.push_back(*p);
            else if(drawingPoints.size()==1)//正在选第二个点
            {
                drawingPoints.push_back(*p);
                double a=abs(drawingPoints[1].first-drawingPoints[0].first)/2;
                double b=abs(drawingPoints[1].second-drawingPoints[0].second)/2;
                double x=(drawingPoints[1].first+drawingPoints[0].first)/2;
                double y=(drawingPoints[1].second+drawingPoints[0].second)/2;
                if(currentMode==_OVAL_)
                    drawOval(x,y,a,b);
                else
                    drawFilledOval(x,y,a,b);
                curPicID++;
            }
            else if(drawingPoints.size()==2)
            {
                clean_drawingPoints();
                drawingPoints.push_back(*p);
            }
        }
    }
    else if(currentMode==_MOVE_)
    {
        mid_x=event->localPos().x();
        mid_y=this->height()-event->localPos().y();
    }
    else if(currentMode==_CHOOSE_)
    {
        if(event->button()==Qt::LeftButton)
            choose(x,y);
    }
    update();
}

void Draw::mouseMoveEvent(QMouseEvent *event)
{
    double x=event->localPos().x(),y=this->height()-event->localPos().y();
    if(currentMode==_POINT_)
    {
        if(LeftButtonPressed)
            drawPoint(x,y);
    }
    else if(currentMode==_LINE_)
    {
        ;
    }
    else if(currentMode==_CURVE_)
    {
        ;
    }
    else if(currentMode==_RECT_)
    {
        ;
    }
    else if(currentMode==_FILLEDRECT_)
    {
        ;
    }
    else if(currentMode==_POLY_)
    {
        ;
    }
    else if(currentMode==_FILLEDPOLY_)
    {
        ;
    }
    else if(currentMode==_MOVE_)
        if(LeftButtonPressed)
            move(x-mid_x,y-mid_y);
    mid_x=event->localPos().x();
    mid_y=this->height()-event->localPos().y();
    update();
}

void Draw::mouseReleaseEvent(QMouseEvent *event)
{
    LeftButtonPressed = false;
    RightButtonPressed = false;
    if(event->button() == Qt::LeftButton && currentMode == _POINT_)
        curPicID++;
    else if(event->button() == Qt::LeftButton && currentMode == _MOVE_)
    {
        double to_x,to_y;
        to_x=event->localPos().x();
        to_y=height()-event->localPos().y();
        move(to_x-from_x,to_y-from_y);
    }

    update();
    //QMessageBox::about(NULL, "About", QString::number(event->localPos().x())+" "+QString::number(this->height()-event->localPos().y()));
}

void Draw::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        if(currentMode==_POLY_)
        {
            drawPoly();
            curPicID++;
            PolyDrawn=true;
        }
        if(currentMode==_FILLEDPOLY_)
        {
            drawFilledPoly();
            curPicID++;
            PolyDrawn=true;
        }
    }
}

