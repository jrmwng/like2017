# like2017

## [simd_string.h](like2017/simd_string.h)
SIMD implementations of fixed-length null-terminated string

    jrmwng::avx_string<char> strJeremy("Jeremy");
    jrmwng::avx_string<char> strCally ("Cally");
    
    if (strCally < strJeremy) // should be faster than 'strcmp(strCally, strJeremy) < 0'
    {
        std::cout << strCally << " is before " << strJeremy << std::endl;
    }

## [linq.h](like2017/linq.h)
LINQ alike query interface written in C++ template

    for (auto tuple : jrmwng::linq::from(vectorStudent)
        .where([](auto const & student)
        {
            return student.gender == 'M';
        })
        .order_by([](auto const & student)
        {
            return student.name;
        })
        .select([](auto const & student)
        {
            return std::make_tuple(student.name, student.address);
        }))
    {
        std::cout << std::get<0>(tuple) << '@' << std::get<1>(tuple) << std::endl;
    }
