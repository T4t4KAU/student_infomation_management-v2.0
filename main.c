#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define STR_NAME_LEN 128
#define STR_NUMBER_LEN 32
#define SEARCH_BY_NUMBER 1
#define SEARCH_BY_NAME 2
#define TOTAL_COURSE_NUM 16
#define COURSE_FILE_PATH "course_info.txt"
#define STUDENT_FILE_PATH "student_info.txt"
#define LINE "-----------------------------------------\n"

typedef struct
{
    char name[STR_NAME_LEN];
    char number[STR_NUMBER_LEN];
    int num_courses;
    int select_courses;
    int* score_courses;
} student_info;

typedef student_info info;

typedef struct
{
    char name[STR_NAME_LEN];
} course_info;

typedef enum
{
   READ_FOR_SEARCH,
   READ_FOR_ALL
} read_type;

course_info courseList[TOTAL_COURSE_NUM];

// 打印所有课程信息
void printCourses()
{
    printf("课程目录\n");
    for(int i=0;i < TOTAL_COURSE_NUM;i++)
    {
        printf("%d. %s\n",i+1,courseList[i].name);
    }
}

// 录入总课程信息
void inputCourses()
{
    FILE *fp = NULL;
    fp = fopen(COURSE_FILE_PATH,"w+");
    printf("共须录入%d门课程\n",TOTAL_COURSE_NUM);
    for(int i = 0; i < TOTAL_COURSE_NUM;i++)
    {
        printf("%d. ",i+1);
        scanf("%s",courseList[i].name);
    }
    for(int i = 0; i < TOTAL_COURSE_NUM;i++)
    {
        fprintf(fp,"%s\n",courseList[i].name);
    }
    fclose(fp);
}

// 标识执行结果
typedef enum
{
    INSERT_SUCCESS,
    INSERT_FAILURE,
    DELETE_SUCCESS,
    DELETE_FAILURE,
    MODIFY_SUCCESS,
    MODIFY_FAILURE,
    SEARCH_SUCCESS,
    SEARCH_FAILURE
} execute_result;

// 标识查找类型
typedef enum
{
    SEARCH_SINGLE_NODE,
    SEARCH_NODE_LIST
} search_type;

typedef struct LinkedList
{
    info data;
    struct LinkedList *next;
} node;

// 创建链表节点
node* createNode()
{
    return (node*)calloc(1,sizeof(node));
}

// 创建链表头节点
node* createList()
{
    node* headNode = createNode();
    headNode->next = NULL;
    return headNode;
}

// 插入节点
execute_result insertNode(node* headNode,node* newNode)
{
    if(newNode == NULL)
        return INSERT_FAILURE;
    newNode->next = headNode->next;
    headNode->next = newNode;
    return INSERT_SUCCESS;
}

// 修改节点数据
execute_result modifyNode(node* LNode,info newData)
{
    if(LNode == NULL)
        return MODIFY_FAILURE;
    LNode->data = newData;
    return MODIFY_SUCCESS;
}


// 删除节点
execute_result deleteNode(node* headNode,node* LNode)
{
    node* prevNode = headNode->next;
    while(prevNode != NULL)
    {
        if(prevNode->next == LNode)
            break;
        prevNode = prevNode->next;
    }
    if(prevNode == NULL)
        return DELETE_FAILURE;
    prevNode->next = LNode->next;
    if(LNode->data.score_courses)
    {
        free(LNode->data.score_courses);
        LNode->data.score_courses = NULL;
    }
    free(LNode);
    LNode = NULL;
    return DELETE_SUCCESS;
}

// 释放整个链表
void freeList(node* headNode)
{
    node* LNode = headNode->next;
    while(LNode != NULL)
    {
        deleteNode(headNode,LNode);
        LNode = LNode->next;
    }
    free(headNode);
    headNode = NULL;
}

// 打印学生信息
void printData(info stuData)
{
    printf(LINE);
    printf("number: %s\n",stuData.number);
    printf("name: %s\n",stuData.name);
    printf("所选课程数: %d\n",stuData.num_courses);
    printf("所选课程及其对应分数:\n");
    int num = 0;
    int select_course = stuData.select_courses;
    for(int i = 0;i < TOTAL_COURSE_NUM;i++)
    {
        if(select_course & 1)
        {
            printf("%d. name:%-50s\tscore:%d\n", i + 1, courseList[i].name,stuData.score_courses[num++]);
        }
        select_course = select_course >> 1;
    }
    printf("\n");
    printf(LINE);
}

// 打印单个节点
void printNode(node* LNode)
{
    printData(LNode->data);
}

// 打印整个链表
void printList(node* List)
{
    printf("\n学生名单\n");
    node* LNode = List->next;
    while(LNode != NULL)
    {
        printNode(LNode);
        LNode = LNode->next;
    }
}

// 设置选课信息
int set_select_course(int num,int select_courses)
{
    return (1 << (num-1)) | select_courses;
}

int selectCourse(int num_courses)
{
    printCourses();
    int select_courses = 0;
    int n;
    for(int i=0;i<num_courses;i++)
    {
        printf("输入课程(%d)序号: ",i+1);
        scanf("%d",&n);
        select_courses = set_select_course(n,select_courses);
    }
    return select_courses;
}

// 获取选择课程数
int get_num_courses(int select_courses)
{
    int num_courses = 0;
    while(select_courses != 0)
    {
        if(select_courses & 1)
            num_courses++;
        select_courses = select_courses >> 1;
    }
    return num_courses;
}

// 录入课程分数
void write_course_score(info* stuData)
{
    printf("录入课程分数\n");
    int num_courses = stuData->num_courses;
    if(num_courses <= 0)
        return;
    int select_courses = stuData->select_courses;
    int num = 0;
    for(int i = 0;i < TOTAL_COURSE_NUM;i++)
    {
        if(select_courses & 1)
        {
            printf("输入[%s]的成绩: ",courseList[i].name);
            scanf("%d",&stuData->score_courses[num++]);
        }
        select_courses = select_courses >> 1;
    }
}

// 根据比较依据进行比较
bool compareNode(node* LNode,info compareInfo,int searchInfo)
{
    if(SEARCH_BY_NUMBER & searchInfo) {
        if(strcmp(compareInfo.number,LNode->data.number) != 0)
            return false;
    }
    if(SEARCH_BY_NAME & searchInfo) {
        if(strcmp(compareInfo.name,LNode->data.name) != 0)
            return false;
    }
    return true;
}

node* searchNode(node* headNode,info compareInfo,int searchInfo)
{
    node* LNode = headNode->next;

    // 循环查找
    while(LNode != NULL)
    {
        if(compareNode(LNode,compareInfo,searchInfo))
        {
            return LNode;
        }
        LNode = LNode->next;
    }
    return NULL;
}

execute_result searchNodeList(node* headNode,node* resultList,info compareInfo,int searchInfo,search_type searchType)
{
    node* LNode = headNode->next;
    int num_result = 0;

    // 循环查找
    while(LNode != NULL)
    {
        if(compareNode(LNode,compareInfo,searchInfo))
        {
            node* resultNode = createNode();
            resultNode->data = LNode->data;
            insertNode(resultList,resultNode);
            if(searchType == SEARCH_SINGLE_NODE)
                return SEARCH_SUCCESS;
            num_result++;
        }
        LNode = LNode->next;
    }
    if(num_result == 0)
    {
        resultList = NULL;
        return SEARCH_FAILURE;
    }
    resultList->data.num_courses = num_result;
    return SEARCH_SUCCESS; // 返回结果集
}

info readInfo(read_type type)
{
    info newData;
    printf(LINE);
    printf("输入学生学号: ");
    scanf("%s",newData.number);
    printf("输入学生姓名: ");
    scanf("%s",newData.name);
    if(type == READ_FOR_SEARCH)
        return newData;
    printf("修改选课信息\n");
    printf("输入选课数目: ");
    scanf("%d",&newData.num_courses);
    if(newData.num_courses == 0)
    {
        if(newData.score_courses != NULL)
        {
            free(newData.score_courses);
            newData.score_courses = NULL;
        }
        return newData;
    }
    newData.select_courses = selectCourse(newData.num_courses);
    newData.score_courses = (int*)calloc(newData.num_courses,sizeof(int));
    write_course_score(&newData);
    printf(LINE);
    return newData;
}

int select_search_info(info stuInfo)
{
    int searchInfo = 0;
    if(strcmp(stuInfo.number,"?") != 0)
    {
        searchInfo |= SEARCH_BY_NUMBER;
    }
    if(strcmp(stuInfo.name,"?") != 0)
    {
        searchInfo |= SEARCH_BY_NAME;
    }
    return searchInfo;
}

void insertInfo(node* List)
{
    info newData = readInfo(READ_FOR_ALL);
    node* newNode = createNode();
    newNode->data = newData;
    insertNode(List,newNode);
}

void deleteInfo(node* List)
{
    printf("输入待删除学生信息(至少填一项,未知信息填'?')\n");
    info stuInfo = readInfo(READ_FOR_SEARCH);
    int searchInfo = select_search_info(stuInfo);
    node* LNode = searchNode(List,stuInfo,searchInfo);
    if(LNode != NULL)
    {
        if(deleteNode(List,LNode) == DELETE_SUCCESS)
        {
            printf("删除成功\n");
        }
        else
        {
            printf("删除失败\n");
        }
    }
    else
    {
        printf("该学生不存在\n");
    }
}

// 修改学生选课信息
void modifyInfo(node* List)
{
    printf("输入待修改学生的信息\n");
    info stuInfo = readInfo(READ_FOR_SEARCH);
    int searchInfo = select_search_info(stuInfo);
    node* LNode = searchNode(List,stuInfo,searchInfo);
    if(LNode != NULL)
    {
        info newData = LNode->data;
        printf("输入课程数: ");
        scanf("%d",&newData.num_courses);
        newData.select_courses = selectCourse(newData.num_courses);
        free(newData.score_courses);
        newData.score_courses = (int*)calloc(newData.num_courses,sizeof(int));
        write_course_score(&newData);
        modifyNode(LNode,newData);
        printf("修改成功\n");
    }
    else
    {
        printf("未查找到该学生\n");
    }
}

// 查找学生信息
void searchInfo(node* List)
{
    printf("输入待查找学生的信息\n");
    info stuInfo = readInfo(READ_FOR_SEARCH);
    int searchInfo = select_search_info(stuInfo);
    node* resultList = (node*)malloc(sizeof(node));
    searchNodeList(List,resultList,stuInfo, searchInfo, SEARCH_NODE_LIST);
    if(resultList == NULL)
    {
        printf("未查找到任何信息\n");
        return;
    }
    printList(resultList);
    freeList(List);
}

// 读取学生信息文件
void readStudentInfo(node* studentList)
{
    FILE* fp = NULL;
    info newData = {0};
    fp = fopen(STUDENT_FILE_PATH,"r");
    if(fp == NULL)
        return;
    while(fscanf(fp,"%s\t%s\t%d\t",newData.number,newData.name,&newData.select_courses) != EOF)
    {
        newData.num_courses = get_num_courses(newData.select_courses);
        newData.score_courses = NULL;
        if(newData.num_courses > 0)
        {
            newData.score_courses = (int*)malloc(newData.num_courses*sizeof(int));
            for(int i = 0;i < newData.num_courses;i++)
                fscanf(fp,"%d\t",&newData.score_courses[i]);
        }
        fscanf(fp,"\n");
        node* newNode = createNode();
        newNode->data = newData;
        insertNode(studentList,newNode);
    }
    fclose(fp);
}

// 读取课程信息文件
void readCourseInfo()
{
    FILE* fp = NULL;
    course_info course;
    int num = 0;
    fp = fopen(COURSE_FILE_PATH,"r");
    if(fp  == NULL)
        return;
    while(fscanf(fp,"%s\n",course.name) != EOF)
    {
        courseList[num++] = course;
    }
    fclose(fp);
}

// 菜单
bool menu(node* List)
{
    int c;
    printf(LINE);
    printf("功能菜单\n");
    printf("1.增加学生个人信息\n2.删除学生个人信息\n3.修改学生课程信息\n4.查找学生个人信息\n5.打印所有学生信息\n6.打印所有课程信息\n7.录入课程信息\n8.退出\n");
    printf("input your choice: ");
    scanf("%d",&c);
    printf(LINE);

    switch(c) {
        case 1:
            insertInfo(List);
            break;
        case 2:
            deleteInfo(List);
            break;
        case 3:
            modifyInfo(List);
            break;
        case 4:
            searchInfo(List);
            break;
        case 5:
            printList(List);
            break;
        case 6:
            printCourses();
            break;
        case 7:
            inputCourses();
            break;
        case 8:
            printf("Bye\n");
            return false;
        default:
            printf("输入有误\n");
    }
    printf(LINE);
    printf("按任意键继续...");
    getchar();
    getchar();
    system("clear");
    printf("\n");
    return true;
}

// 保存链表到文件
void saveStudentList(node* List)
{
    FILE *fp = NULL;
    fp = fopen(STUDENT_FILE_PATH,"w+");
    if(fp == NULL)
    {
        printf("文件打开失败\n");
    }
    node* LNode = List->next;
    while(LNode != NULL)
    {
        fprintf(fp,"%s\t%s\t%d\t",LNode->data.number,LNode->data.name,LNode->data.select_courses);
        if(LNode->data.score_courses)
        {
            for (int i = 0; i < LNode->data.num_courses; i++)
                fprintf(fp, "%d\t", LNode->data.score_courses[i]);
        }
        fprintf(fp,"\n");
        LNode = LNode->next;
    }
    fclose(fp);
}

int main(int argc,char* argv[])
{
    node *studentList = createList();
    readCourseInfo();
    readStudentInfo(studentList);
    while(menu(studentList));
    saveStudentList(studentList);
    return 0;
}
