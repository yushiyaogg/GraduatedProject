/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/15/12 15:21:58
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yushiyao (), yushiyaogg@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */
#include "head.h"
#include "DataSet.h"
#include "BusinessController.h"

int main(int argc, char **argv)
{

    string teststr = "test";
    vector<string> testvect;
    class BusinessController *BC = new BusinessController(teststr);
    class DataSet *DataSet1 = BC -> InitialDataSet(teststr);
    DataSet *DataSet2 = DataSet1->map(teststr,testvect);
    DataSet *DataSet3 = DataSet2->shuffle();
    DataSet *DataSet4 = DataSet3 -> map(teststr,testvect);
    testvect = DataSet3 -> reduce();
    cout << "123"<< endl;

    /*   if(argc != 2)
    {
        cout << "input the IP address"<<endl;
        return -1;
    }
    if((m_epoll.epoll_init()) < 0)
        return -1;
    S_Agent* s_agent = new S_Agent();
    if((s_agent -> connect_server(argv[1])) < 0)
    {
        cerr <<"connect_server error"<<errno<<endl;
        delete s_agent;
        return -1;
    }
    struct mesg_head head;
    head.NewOpcode = MAPOP;
    s_agent->Writebuff.add_buff(&head,sizeof(mesg_head));
    if((m_epoll.epollwait()) < 0)
    {
        delete s_agent;
        return -1;
    }
    delete s_agent;
    return 0;*/
}