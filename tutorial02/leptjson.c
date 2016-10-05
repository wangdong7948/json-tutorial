#include "leptjson.h"
#include <assert.h>  /* assert() */
#include <stdlib.h>  /* NULL, strtod() */
//
#include <math.h>  //HUGE_VAL
#include<errno.h>   //errno,ERANGE

#define EXPECT(c, ch)       do { assert(*c->json == (ch)); c->json++; } while(0)
//
#define ISDIGIT(ch)          ((ch)>='0'&& (ch)<='9')
#define ISDIGIT09(ch)       ((ch)>='1' && (ch) <='9')


typedef struct {
    const char* json;
}lept_context;

static void lept_parse_whitespace(lept_context* c) {
    const char *p = c->json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        p++;
    c->json = p;
}
/*
//
static int lept_parse_true(lept_context* c, lept_value* v) {
    EXPECT(c, 't');
    if (c->json[0] != 'r' || c->json[1] != 'u' || c->json[2] != 'e')
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 3;
    v->type = LEPT_TRUE;
    return LEPT_PARSE_OK;
}
//
static int lept_parse_false(lept_context* c, lept_value* v) {
    EXPECT(c, 'f');
    if (c->json[0] != 'a' || c->json[1] != 'l' || c->json[2] != 's' || c->json[3] != 'e')
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 4;
    v->type = LEPT_FALSE;
    return LEPT_PARSE_OK;
}
//
static int lept_parse_null(lept_context* c, lept_value* v) {
    EXPECT(c, 'n');
    if (c->json[0] != 'u' || c->json[1] != 'l' || c->json[2] != 'l')
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 3;
    v->type = LEPT_NULL;
    return LEPT_PARSE_OK;
}
*/
/*//合并
static int  lept_parse_literal(lept_context* c ,lept_value* v){
    string str[] ={ 't','f','n'};
    v->type t = LEPT_NULL
    EXPECT(c, str);
    if(c->json[0] != number
       return LEPT_PARSE_INVALID_VALUE;
    c->json+=strsize(number);
       v->type = ;
       return LE
    
}
*/
//重构
static int  lept_parse_literal(lept_context* c ,lept_value* v,const char* literal,lept_type type){
    size_t i;
    EXPECT(c,literal[0]);
    for(i = 0;literal[i+1];i++)
        if(c->json[i]!=literal[i+1])
            return LEPT_PARSE_INVALID_VALUE;
    c->json+=i;
    v->type = type;
    return LEPT_PARSE_OK;
}

/*
static int lept_parse_number(lept_context* c, lept_value* v) {
    char* end;
   // TODO validate number 
    v->n = strtod(c->json, &end);
    if (c->json == end)
        return LEPT_PARSE_INVALID_VALUE;
    c->json = end;
    v->type = LEPT_NUMBER;
    return LEPT_PARSE_OK;
}
*/
//重构

//校验数字
static int lept_parse_number(lept_context* c,lept_value* v){
    const char* p = c->json;    //指针p表示当前的解析字符位置
         if(*p == '-') p++;  //负号 跳过
         if(*p=='0') p++;  //单个0  跳过
    else{           
        if(!ISDIGIT09(*p)  return LEPT_PARSE_INVALID_VALUE;     //第一个字符不是1~9  不合法
           for(p++; ISDIGIT(*p);  p++); //多少个整数就跳过多少个
        }
        if(*p == '.'){  //小数点
            p++;    //跳过
            if(!ISDIGIT(*p) return LEPT_PARSE_INVALID_VALUE;  //不是整数 返回错误码
               for(p++;ISDIGIT(*p); p++;    // 先跳过首个整数 ，检查是不是整数，有多少个跳过多少
            }
                   
        if(*p=='e'  || *p=='E'){  //出现E/e,有指数部分
            p++;    //跳过那个e
            if(*p == '+'||*p == '-')  p++; //有一个+/- ， 跳过
                
              if(!ISDIGIT(*p) return LEPT_PARSE_INVALID_VALUE; ////不是整数 返回错误码
                 for(p++;ISDIGIT(*p);p++);       // 先跳过首个整数 ，检查是不是整数，有多少个跳过多少     
            }
          errno = 0;          
          v->n = strtod(c->json,NULL);
          if(errno == ERANGE && (v->n == HUGE_VAL || v->n == -HUGE_VAL))
               return LEPT_PARSE_NUMBER_TOO_BIG;//从返回值得知数值是否过大，以返回 LEPT_PARSE_NUMBER_TOO_BIG 错误码。
          v->type = LEPT_NUMBER;
          c->json = p;
          return LEPT_PARSE_OK;
             
}

/*
static int lept_parse_value(lept_context* c, lept_value* v) {
    switch (*c->json) {
        case 't':  return lept_parse_true(c, v);
        case 'f':  return lept_parse_false(c, v);
        case 'n':  return lept_parse_null(c, v);
        default:   return lept_parse_number(c, v);
        case '\0': return LEPT_PARSE_EXPECT_VALUE;
    }
*/
//改  重构之后
 static int lept_parse_value(lept_context* c, lept_value* v){
    switch(*c->json){
        case 't':  return lept_parse_literal(c, v,"true",LEPT_TRUE);
        case 'f':  return lept_parse_literal(c, v,"false",LEPT_FALSE);
        case 'n':  return lept_parse_literal(c, v,"null",LEPT_NULL);
        default:   return lept_parse_number(c, v);
        case '\0': return LEPT_PARSE_EXPECT_VALUE;
    }
 
 }
    
    
}

int lept_parse(lept_value* v, const char* json) {
    lept_context c;
    int ret;
    assert(v != NULL);
    c.json = json;
    v->type = LEPT_NULL;
    lept_parse_whitespace(&c);
    if ((ret = lept_parse_value(&c, v)) == LEPT_PARSE_OK) {
        lept_parse_whitespace(&c);
        if (*c.json != '\0') {
            v->type = LEPT_NULL;
            ret = LEPT_PARSE_ROOT_NOT_SINGULAR;
        }
    }
    return ret;
}

lept_type lept_get_type(const lept_value* v) {
    assert(v != NULL);
    return v->type;
}

double lept_get_number(const lept_value* v) {
    assert(v != NULL && v->type == LEPT_NUMBER);
    return v->n;
}
