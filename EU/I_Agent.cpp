/*
 * =====================================================================================
 *
 *       Filename:  I_Agent.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/12/12 15:04:39
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */
#include "I_Agent.h"

I_Agent::I_Agent(int I_fd)
{
    i_manager_agent.i_agent = this;
    this ->fd = I_fd;
    read_len = MSGHEAD_LEN;
    Readbuff_head.new_buff(MSGHEAD_LEN);
    Readbuff = &Readbuff_head;
    read_stat = READ_MESGHEAD;

}

I_Agent::~I_Agent()
{
    if(m_epoll.epoll_delete(this) < 0)
    {
        cout<< "epoll_delete error"<<endl;
    }
    i_manager_agent.i_agent = NULL;
    close(fd);
}

int I_Agent::getfd()
{
    return fd;
}


int I_Agent::readagent()
{
    int ret;
    for(;;)
    {
        ret = I_read.read_socket(fd,Readbuff->bufferptr + Readbuff->buff_length - read_len,&read_len);

        if(ret == READ_ERROR)
        {
            cout << "read_socket error"<<endl;
            delete this;
            return -1;
        }
        else if(ret == READ_END)
        {
            cout <<"TCP disconnect"<< endl;
            delete this;
            return 0;
        }
        else if(ret == READ_BLOCK)
        {
            if(Writebuff.head!= NULL)
            {
                if(m_epoll.epoll_modify(EPOLLOUT,this) < 0)
                {
                    cout << "epoll_modify error"<<endl;
                    delete this;
                    return -1;
                }
            }
            return 0;
        }
        else if(ret == READ_SUCCESS)
        {
            if(package_head_data() < 0)
            {
                cout << "package_head_data error"<< endl;
                return -1;
            }
        }
    }

}

int I_Agent::writeagent()
{
    int ret;
    ret = I_write. write_socket(fd,&Writebuff);
    if(ret == WRITE_ERROR)
    {
        cout << "write_socket error"<< endl;
        delete this;
        return -1;
    }
    else if(ret == WRITE_BLOCK)
    {
            return 0;
    }
    else if(ret == WRITE_SUCCESS)
    {
        if(m_epoll.epoll_modify(EPOLLIN,this) < 0)
        {
            cout << "epoll_modify error"<< endl;
            return -1;
        }
        else
            return 0;
    }
}

int I_Agent::package_head_data()
{
    if(read_stat == READ_MESGHEAD)
    {
        Head = (struct mesg_head*)(Readbuff_head.bufferptr);
        Readbuff_data.delete_buff();
        if(Head -> length>0)
        {
            Readbuff_data.new_buff(Head -> length);
            read_len = Head -> length;
            
            Readbuff = &Readbuff_data;
            read_stat = READ_DATA;
            return 0;
        }
    }
    if(exec() < 0) 
    {
        cout << "i_manager_agent.exec error"<<endl;
        return -1;
    }
    else
    {
        read_len = MSGHEAD_LEN;
        read_stat = READ_MESGHEAD;
        Readbuff = &Readbuff_head;
        return 0;
    }
}
int I_Agent::exec()
{
      /*  memcpy(BHSbuff.bufferptr,i_agent->Readbuff_head.bufferptr,BHS_LEN);
        pdu.bhs = (struct iscsi_hdr*)(Msgheadbuff.bufferptr);
        pdu.Datasize =(( pdu.bhs->datalength[0]<<16) +(pdu.bhs->datalength[1]<<8)+pdu.bhs->datalength[2]);
        Databuff.delete_buff();
        if(pdu.Datasize > 0)
        {   
            Databuff.new_buff(pdu.Datasize);
            memcpy(Databuff.bufferptr,i_agent->Readbuff_data.bufferptr,pdu.Datasize);
        }  */ 
        if(0 > cmnd_exec())
        {   
            cout <<"cmnd_exec error"<< endl;
            return -1; 
        }   
        return 0;
}
int I_Agent::cmnd_exec()
{
    if(Head->cmd == MSG_BC_EU_INIT_DATA)
    {   
        cout << "MSG TYPE: INIT DATA, LENGTH:"<< Head->length<< endl;

        bc_eu::pb_MSG_BC_EU_INIT_DATA Initoperation;
        char* Data = (char*)Readbuff_data.bufferptr;
        string load(Data,Head->length);
        if(Initoperation.ParseFromArray((void*)Data,Head->length) < 0)
            cout << "parseerror"<< endl;
        string splitname = Initoperation.splitname();
        int    splitnumber = Initoperation.splitnumber();

        string splitid = splitname + IntToString(splitnumber);
        
        //datacontainer.insert(splitid);
        cout << splitname << endl;
        cout << splitnumber<<endl;
        cout << splitid << endl;

        struct mesg_head responsehead;
        responsehead.cmd = MSG_BC_EU_INIT_DATA_ACK;
        responsehead.error = 0;
        responsehead.length = 0;
        Writebuff.add_buff(&responsehead,MSGHEAD_LEN);
        //delete temp;
        //see_datacontainer(datacontainer);
    }
/*    if(ptr.cmd == MSG_BC_EU_MAP)
        {
            cout << "MSG TYPE: MAPOP, LENGTH:"<< ptr.length<< endl;
            char *temp = new char[ptr.length];

            number = read(connfdcontainer.at(i),temp,ptr.length);
            while (number < 0)
            {
                number = read(connfdcontainer.at(i),temp,ptr.length);

            }
            if(number != ptr.length)
            {
                cout << "read error"<< endl;
            }
            number =-1;
            bc_eu::pb_MSG_BC_EU_MAP Mapoperation;
            string load(temp,ptr.length);
            Mapoperation.ParseFromString(load);

            string sourcedata =Mapoperation.sourcesplitname();
            int sourcedatasplitnumber = Mapoperation.sourcesplitnumber();
            string destdata = Mapoperation.destsplitname();
            int destdatasplitnumber = Mapoperation.destsplitnumber();
            string sourcedataid = sourcedata+ IntToString(sourcedatasplitnumber) ;
            string destdataid   = destdata + IntToString(destdatasplitnumber);

            cout << "sourcedataid"<<sourcedataid<< endl;
            cout << "destdataid" <<destdataid << endl;
            struct mesg_head responsemap_ptr;
            responsemap_ptr.cmd = MSG_BC_EU_MAP_ACK;
            responsemap_ptr.error = -1;
            responsemap_ptr.length = 0;
            set<string>::iterator tempit;
            if((tempit = datacontainer.find(sourcedataid)) != datacontainer.end())
            {
                responsemap_ptr.error = 0;
                datacontainer.insert(destdataid);
            }
            write(connfdcontainer.at(i),&responsemap_ptr,20);
            delete temp;
            see_datacontainer(datacontainer);
        }
    if(ptr.cmd == MSG_BC_EU_DELETE_DATA)
        {
            cout << "MSG TYPE: DELETEDATA, LENGTH:"<< ptr.length<< endl;
            char *temp = new char[ptr.length];
            number = read(connfdcontainer.at(i),temp,ptr.length);
            while(number < 0)
            {
                number = read(connfdcontainer.at(i),temp,ptr.length);
            }
            if(number != ptr.length)
            {
                cout << "read error"<< endl;
            }
            number  = -1;
            bc_eu::pb_MSG_BC_EU_DELETE_DATA Deleteoperation;
            string load(temp,ptr.length);
            Deleteoperation.ParseFromString(load);

            string deletesourcedata = Deleteoperation.sourcesplitname();
            int deletesourcedatanumber = Deleteoperation.sourcesplitnumber();
            string deletedataid = deletesourcedata + IntToString(deletesourcedatanumber);
            cout << deletedataid<< endl;
            if(datacontainer.find(deletedataid) != datacontainer.end())
                datacontainer.erase(datacontainer.find(deletedataid));
            struct mesg_head responsedelete_ptr;
            responsedelete_ptr.cmd = MSG_BC_EU_MAP_ACK;
            responsedelete_ptr.error = 0;
            responsedelete_ptr.length = 0;
            write(connfdcontainer.at(i),&responsedelete_ptr,20);
            delete temp;
            see_datacontainer(datacontainer);
        }
    if(ptr.cmd == MSG_BC_EU_REDUCE)
        {
            cout << "MSG TYPE: REDUCE, LENGTH:"<< ptr.length<< endl;
            char *temp = new char[ptr.length];

            number = read(connfdcontainer.at(i),temp,ptr.length);
            while (number < 0)
            {
                number = read(connfdcontainer.at(i),temp,ptr.length);

            }
            if(number != ptr.length)
            {
                cout << "read error"<< endl;
            }
            number =-1;
            bc_eu::pb_MSG_BC_EU_REDUCE Reduceoperation;
            string load(temp,ptr.length);
            Reduceoperation.ParseFromString(load);

            string sourcedata =Reduceoperation.sourcesplitname();
            int sourcedatasplitnumber = Reduceoperation.sourcesplitnumber();
            string sourcedataid = sourcedata+ IntToString(sourcedatasplitnumber) ;

            cout << "sourcedataid"<<sourcedataid<< endl;

            struct mesg_head responsereduce_ptr;
            responsereduce_ptr.cmd = MSG_BC_EU_REDUCE_ACK;
            responsereduce_ptr.error = -1;
            responsereduce_ptr.length = 0;

            set<string>::iterator tempit;
            string temp_str;
            char* body;
            if((tempit = datacontainer.find(sourcedataid)) != datacontainer.end())
            {
                responsereduce_ptr.error = 0;
                bc_eu::pb_MSG_BC_EU_REDUCE_ACK mesg_body;

                    bc_eu::pb_MSG_BC_EU_REDUCE_ACK_result *result;
                    result = mesg_body.add_result_list();
                    result -> set_key("1");
                    result -> set_value("1,2");

                mesg_body.SerializeToString(&temp_str);
                responsereduce_ptr.length = temp_str.length();
                body = new char[temp_str.size()];
                memcpy(body,temp_str.c_str(),temp_str.length());

            }
            write(connfdcontainer.at(i),&responsereduce_ptr,20);
            write(connfdcontainer.at(i),body,temp_str.length());
            delete body;
            see_datacontainer(datacontainer);
        }
    if(ptr.cmd == MSG_BC_EU_SHUFFLE)
        {
            cout << "MSG TYPE: SHUFFLE,LENGTH:"<< ptr.length<< endl;
            char* temp = new char[ptr.length];
            number = read(connfdcontainer.at(i),temp,ptr.length);
            while(number < 0)
            {
                number = read(connfdcontainer.at(i),temp,ptr.length);
            }
            if(number != ptr.length)
            {
                cout <<"read error"<< endl;
            }
            number = -1;
            bc_eu::pb_MSG_BC_EU_SHUFFLE Shuffleoperation;
            string load(temp,ptr.length);
            Shuffleoperation.ParseFromString(load);

            string sourcedata = Shuffleoperation.sourcesplitname();
            int    sourcedatanumber = Shuffleoperation.sourcesplitnumber();
            
            string destdata = Shuffleoperation.destsplitname();
            int    destnumber = Shuffleoperation.destsplitnumber();
            string destdataid;
            string localIP = LOCALIP;
            string sourcedataid = sourcedata + IntToString(sourcedatanumber);
            cout <<"sourcedataid" <<sourcedataid << endl;
            struct mesg_head responseshuffle_ptr;
            responseshuffle_ptr.cmd = MSG_BC_EU_SHUFLLE_ACK;
            responseshuffle_ptr.error = -2;
            responseshuffle_ptr.length = 0;
            for(int j = 0; j < datacontainer.size(); j ++)
            {
                if(datacontainer.find(sourcedataid) != datacontainer.end())
                {
                    responseshuffle_ptr.error = -1;
                    for(int i = 0; i < Shuffleoperation.ipinfolist_size();i ++)
                    {
                        string IP = Shuffleoperation.ipinfolist(i).ip();
                        int key = (int)(Shuffleoperation.ipinfolist(i).key());
                        if(IP == localIP)
                        {
                            destdataid = destdata + IntToString(key);
                        }
                        datacontainer.insert(destdataid);
                    }
                    break;

                }
            }
            write(connfdcontainer.at(i),&responseshuffle_ptr,20);
            delete temp;
            see_datacontainer(datacontainer);
        }
*/

}
string I_Agent::IntToString(int number)
{
        stringstream ss; 
        ss << number;
        string temp = ss.str();
        return temp;
}