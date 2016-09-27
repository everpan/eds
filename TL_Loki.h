/* 
 * File:   TL_Loki.h
 * Author: everpan
 *
 * Created on 2011年6月20日, 下午2:49
 */

#ifndef TL_LOKI_H
#define	TL_LOKI_H
namespace tidp {
    class NullType;

    class EmptyType {
    };

    template<class T>
    struct Type2Type {
        typedef T OrigType;
    };

    template<int v>
    struct Int2Type {

        enum {
            value = v
        };
    };

    template<class Head, class Tail>
    struct TypeList {
        typedef Head H;
        typedef Tail T;
    };

#define TYPELIST_1(T1) TypeList<T1,tidp::NullType>
#define TYPELIST_2(T1, T2) TypeList<T1, tidp::TYPELIST_1(T2)>
#define TYPELIST_3(T1, T2, T3) TypeList<T1, tidp::TYPELIST_2(T2, T3)>
#define TYPELIST_4(T1, T2, T3, T4) TypeList<T1, tidp::TYPELIST_3(T2, T3, T4)>
#define TYPELIST_5(T1, T2, T3, T4, T5) TypeList<T1, tidp::TYPELIST_4(T2, T3, T4, T5)>
#define TYPELIST_6(T1, T2, T3, T4, T5, T6) TypeList<T1, tidp::TYPELIST_5(T2, T3, T4, T5, T6)>
#define TYPELIST_7(T1, T2, T3, T4, T5, T6, T7) TypeList<T1, tidp::TYPELIST_6(T2, T3, T4, T5, T6, T7)>
#define TYPELIST_8(T1, T2, T3, T4, T5, T6, T7, T8) TypeList<T1, tidp::TYPELIST_7(T2, T3, T4, T5, T6, T7, T8)>
#define TYPELIST_9(T1, T2, T3, T4, T5, T6, T7, T8, T9) TypeList<T1, tidp::TYPELIST_8(T2, T3, T4, T5, T6, T7, T8, T9)>
#define TYPELIST_10(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10) TypeList<T1, tidp::TYPELIST_9(T2, T3, T4, T5, T6, T7, T8, T9, T10)>
#define TYPELIST_11(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11) TypeList<T1, tidp::TYPELIST_10(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11)>
#define TYPELIST_12(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12) TypeList<T1, tidp::TYPELIST_11(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12)>
#define TYPELIST_13(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13) TypeList<T1, tidp::TYPELIST_12(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13)>
#define TYPELIST_14(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14) TypeList<T1, tidp::TYPELIST_13(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14)>
#define TYPELIST_15(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15) TypeList<T1, tidp::TYPELIST_14(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15)>
#define TYPELIST_16(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16) TypeList<T1, tidp::TYPELIST_15(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16)>
#define TYPELIST_17(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17) TypeList<T1, tidp::TYPELIST_16(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17)>
#define TYPELIST_18(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18) TypeList<T1, tidp::TYPELIST_17(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18)>
#define TYPELIST_19(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19) TypeList<T1, tidp::TYPELIST_18(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19)>
#define TYPELIST_20(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20) TypeList<T1, tidp::TYPELIST_19(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20)> 

    template<class TList> struct Length;
    template<>
    struct Length<NullType>
    {
        enum { value = 0 };
    };
    template<class Head, class Tail>
    struct Length<TypeList<Head, Tail> >
    {
        enum { value = 1 + Length<Tail>::value };
    };
    
    
    template<class TList, unsigned int i> struct TypeAt;
    template<class Head, class Tail> struct TypeAt<TypeList<Head, Tail>, 0>
    {
        typedef Head Result;
    };
    template<class Head, class Tail, unsigned int i> struct TypeAt<TypeList<Head, Tail>, i>
    {
        typedef typename TypeAt<Tail, i-1>::Result Result;
    };

    template<class TList, unsigned int index, typename DefaultType = NullType> struct TypeAtNonStrict
    {
        typedef DefaultType Result;
    };
    template <class Head, class Tail, typename DefaultType> struct TypeAtNonStrict<TypeList<Head, Tail>, 0, DefaultType>
    {
        typedef Head Result;
    };
    template <class Head, class Tail, unsigned int i, typename DefaultType> struct TypeAtNonStrict<TypeList<Head, Tail>, i, DefaultType>
    {
        typedef typename TypeAtNonStrict<Tail, i - 1, DefaultType>::Result Result;
    };
    
    /**
     * 取链表上类型为T的序号, 没有则返回-1
     */
    template<class TList, class T> struct IndexOf;
    template<class T> struct IndexOf<NullType, T>
    {
        enum { value = -1 };
    };
    template<class Tail, class T> struct IndexOf<TypeList<T,Tail>, T>
    {
        enum { value = 0 };
    };
    template<class Head, class Tail, class T> struct IndexOf<TypeList<Head, Tail>, T>
    {
    private:
        enum { temp = IndexOf<Tail, T>::value };
    public:
        enum { value = temp == -1 ? -1 : 1 + temp };
    };

    /**
     * Append, 添加到链表尾部
     */
    template<class TList, class T> struct Append;
    template<> struct Append<NullType, NullType>
    {
        typedef NullType Result;
    };
    template<class T> struct Append<NullType,T>
    {
        typedef TYPELIST_1(T) Result;
    };
    template<class Head, class Tail> struct Append<NullType, TypeList<Head, Tail> >
    {
        typedef TypeList<Head, Tail> Result;
    };
    template<class Head, class Tail, class T> struct Append<TypeList<Head, Tail>, T>
    {
        typedef TypeList<Head, typename Append<Tail, T>::Result> Result;
    };

    /**
     * Erase 删除
     */
    template<class TList, class T> struct Erase;
    template<class T> struct Erase<NullType, T>
    {
        typedef NullType Result;
    };
    template<class T, class Tail> struct Erase<TypeList<T, Tail>, T>
    {
        typedef Tail Result;
    };
    template<class Head, class Tail, class T> struct Erase<TypeList<Head, Tail>, T>
    {
        typedef TypeList<Head, typename Erase<Tail, T>::Result> Result;
    };

    /**
     * EraseAll 删除
     */
    template<class TList, class T> struct EraseAll;
    template<class T> struct EraseAll<NullType, T>
    {
        typedef NullType Result;
    };
    template<class T, class Tail> struct EraseAll<TypeList<T, Tail>, T>
    {
        typedef typename EraseAll<Tail, T>::Result Result;
    };
    template<class Head, class Tail, class T> struct EraseAll<TypeList<Head, Tail>, T>
    {
        typedef TypeList<Head, typename EraseAll<Tail, T>::Result> Result;
    };
    
    //TLMaker 生成TypeList类型
    template<class T1=NullType,  class T2=NullType,  class T3=NullType,  class T4=NullType,  class T5=NullType,
             class T6=NullType,  class T7=NullType,  class T8=NullType,  class T9=NullType,  class T10=NullType,
             class T11=NullType, class T12=NullType, class T13=NullType, class T14=NullType, class T15=NullType,
             class T16=NullType, class T17=NullType, class T18=NullType, class T19=NullType, class T20=NullType>
    struct TLMaker
    {
    private:
        typedef TYPELIST_20(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10,
                            T11, T12, T13, T14, T15, T16, T17, T18, T19, T20) TmpList;
    public:
        typedef typename EraseAll<TmpList, NullType>::Result Result;
    };
    
    
    //类型提取
    template<typename T>
    class TypeTraits
    {
    private:

        ///////////////////////////////////////////////////////
        //提取引用的原始类型(即去掉引用类型)
        template<class U>
        struct ReferenceTraits
        {
            enum { result = false };
            typedef U Result;
        };

        template<class U>
        struct ReferenceTraits<U&>
        {
            enum { result = true };
            typedef U Result;
        };

    	///////////////////////////////////////////////////////
    	//指针类型
    	template<class U>
    	struct PointerTraits
    	{
    		enum { result = false };
    		typedef tidp::NullType Result;
    	};

    	template<class U>
    	struct PointerTraits<U*>
    	{
    		enum { result = true };
    		typedef U Result;
    	};

    	template<class U>
    	struct PointerTraits<U*&>
    	{
    		enum { result = true };
    		typedef U Result;
    	};

    	///////////////////////////////////////////////////////
    	//成员函数指针, gcc下面支持有问题, 屏蔽之
    	template<typename U>
    	struct PointerToMemberTraits
    	{
    		enum { result = false };
    	};

    	template<class U, class V>
    	struct PointerToMemberTraits<U V::*>
    	{
    		enum { result = true };
    	};

    	template<class U, class V>
    	struct PointerToMemberTraits<U V::*&>
    	{
    		enum { result = true };
    	};

        ///////////////////////////////////////////////////////
        // const
        template<typename U>
        struct UnConstTraits
        {
            enum { result = false };
            typedef U Result;
        };
        template<typename U>
        struct UnConstTraits<const U>
        {
            enum { result = true };
            typedef U Result;
        };
        template<typename U>
        struct UnConstTraits<const U&>
        {
            enum { result = true };
            typedef U& Result;
        };

        ///////////////////////////////////////////////////////
        // volatile
        template<typename U>
        struct UnVolatileTraits
        {
            enum { result = false };
            typedef U Result;
        };
        template<typename U>
        struct UnVolatileTraits<volatile U>
        {
            enum { result = true };
            typedef U Result;
        };
        template<typename U>
        struct UnVolatileTraits<volatile U&>
        {
            enum { result = true };
            typedef U& Result;
        };
    public:
        //T是否是指针类型
    	enum { isPointer        = PointerTraits<T>::result };
        //T是否是引用类型
        enum { isReference 		= ReferenceTraits<T>::result };
        //T是否指向成员函数的指针
        enum { isMemberPointer 	= PointerToMemberTraits<T>::result };

        //T是否是Const类型
        enum { isConst          = UnConstTraits<T>::result };
        //T是否是Volatile类型
        enum { isVolatile       = UnVolatileTraits<T>::result };

        //如果T是指针类型,则获取T的原类型, 即去掉指针类型
    	typedef typename PointerTraits<T>::Result 		    PointeeType;
        //如果T是引用类型,则获取T的原类型, 即去掉引用类型
    	typedef typename ReferenceTraits<T>::Result         ReferencedType;
        //如果T是Const类型,则获取T的原类型, 即去掉Const类型
        typedef typename UnConstTraits<T>::Result           NonConstType;
        //如果T是volatile类型,则获取T的原类型, 即去掉volatile类型
        typedef typename UnVolatileTraits<T>::Result        NonVolatileType;
        //去掉const volatile类型
        typedef typename UnVolatileTraits<typename UnConstTraits<T>::Result>::Result UnqualifiedType;

    public:

    	//////////////////////////////////////////////////////
    	//
    	typedef tidp::TLMaker<unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long>::Result UnsignedInts;
    	typedef tidp::TLMaker<signed char, short, int, long, long long>::Result SignedInts;
    	typedef tidp::TLMaker<bool, char, wchar_t>::Result OtherInts;
    	typedef tidp::TLMaker<float, double, long double>::Result Floats;
        typedef tidp::TYPELIST_2(tidp::EmptyType, tidp::NullType) NullTypes;

        //无符号整形
    	enum { isStdUnsignedInt = tidp::IndexOf<UnsignedInts, T>::value >= 0 };
        //有符号整形
    	enum { isStdSignedInt 	= tidp::IndexOf<SignedInts, T>::value >= 0 };
        //整形
    	enum { isStdInt 		= isStdUnsignedInt || isStdSignedInt || tidp::IndexOf<OtherInts, T>::value >= 0 };
        //浮点类型
    	enum { isStdFloat	 	= tidp::IndexOf<Floats, T>::value >= 0 };
        //数值类型
    	enum { isStdArith	 	= isStdInt || isStdFloat };
        //基础类型(包括void)
    	enum { isStdFundamental	= isStdArith || tidp::IndexOf<tidp::TYPELIST_1(void), T>::value >= 0};
        //空类型
        enum { isNullType       = tidp::IndexOf<NullTypes, T>::value >= 0 };
        //简单类型
        enum { isBaseType       = isStdArith || isPointer || isMemberPointer };

    	//对于复杂类型, 获取数据的引用类型, 即加上引用类型
    	typedef typename TypeSelect<isBaseType, T, ReferencedType&>::Result ReferenceType;

        //对于复杂类型且非空类型, 获取数据的引用类型, 即加上引用类型
        //typedef typename TypeSelect<isBaseType || isNullType, T, ReferencedType&>::Result ReferenceTypeEx;

        //获取数据的原类型, 消除引用的引用这种情况
    	typedef typename TypeSelect<!isReference, T, ReferencedType&>::Result ParameterType;
    };
    
    //boots
    //一元函数
    template <class Operation>
    struct unary_traits_imp;

    template <class Operation>
    struct unary_traits_imp<Operation*> {
        typedef Operation function_type;
        typedef const function_type & param_type;
        typedef typename Operation::result_type result_type;
        typedef typename Operation::argument_type argument_type;
    };

    template <class R, class A>
    struct unary_traits_imp<R(*)(A)> {
        typedef R(*function_type)(A);
        typedef R(*param_type)(A);
        typedef R result_type;
        typedef A argument_type;
    };

    //二元函数
    template <class Operation>
    struct binary_traits_imp;

    template <class Operation>
    struct binary_traits_imp<Operation*> {
        typedef Operation function_type;
        typedef const function_type & param_type;
        typedef typename Operation::result_type result_type;
        typedef typename Operation::first_argument_type first_argument_type;
        typedef typename Operation::second_argument_type second_argument_type;
    };

    template <class R, class A1, class A2>
    struct binary_traits_imp<R(*)(A1, A2)> {
        typedef R(*function_type)(A1, A2);
        typedef R(*param_type)(A1, A2);
        typedef R result_type;
        typedef A1 first_argument_type;
        typedef A2 second_argument_type;
    };


}
#endif	/* TL_LOKI_H */

