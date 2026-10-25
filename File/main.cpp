#include <iostream>
#include <string>
#include <stdexcept>

#include "NumberInclude.h"

using namespace std;

int passCount = 0;
int failCount = 0;


// =========================================================
// �׽�Ʈ �Լ�
// =========================================================

void test(
    const string& name,
    const IrrationalNumber& actual,
    const string& expected
)
{
    string actualString = actual.toString();

    if (actualString == expected)
    {
        cout << "[PASS] "
             << name
             << " -> "
             << actualString
             << endl;

        passCount++;
    }
    else
    {
        cout << "[FAIL] "
             << name
             << " -> ����: "
             << expected
             << ", ����: "
             << actualString
             << endl;

        failCount++;
    }
}


template <typename Function>
void testException(
    const string& name,
    Function function
)
{
    try
    {
        function();

        cout << "[FAIL] "
             << name
             << " -> ���ܰ� �߻����� ����"
             << endl;

        failCount++;
    }
    catch (...)
    {
        cout << "[PASS] "
             << name
             << " -> ���� �߻�"
             << endl;

        passCount++;
    }
}


// =========================================================
// �׽�Ʈ ����
// =========================================================

int main()
{
    cout << "===== �Ϲ� n�� ��ȣ �ڵ� �׽�Ʈ ====="
         << endl
         << endl;


    // =====================================================
    // 1. �⺻ n�� ��ȣ ����
    // =====================================================

    test(
        "���� 5",
        IrrationalNumber(5),
        "5"
    );

    test(
        "sqrt(2)",
        IrrationalNumber(2, 2),
        "sqrt(2)"
    );

    test(
        "sqrt(3)",
        IrrationalNumber(2, 3),
        "sqrt(3)"
    );

    test(
        "cbrt(2)",
        IrrationalNumber(3, 2),
        "root(3,2)"
    );

    test(
        "root(5,7)",
        IrrationalNumber(5, 7),
        "root(5,7)"
    );


    // =====================================================
    // 2. ������ �ܼ�ȭ
    // =====================================================

    test(
        "sqrt(4) = 2",
        IrrationalNumber(2, 4),
        "2"
    );

    test(
        "sqrt(8) = 2sqrt(2)",
        IrrationalNumber(2, 8),
        "2sqrt(2)"
    );

    test(
        "sqrt(12) = 2sqrt(3)",
        IrrationalNumber(2, 12),
        "2sqrt(3)"
    );

    test(
        "sqrt(18) = 3sqrt(2)",
        IrrationalNumber(2, 18),
        "3sqrt(2)"
    );


    // =====================================================
    // 3. �������� �ܼ�ȭ
    // =====================================================

    test(
        "cbrt(8) = 2",
        IrrationalNumber(3, 8),
        "2"
    );

    test(
        "cbrt(16) = 2root(3,2)",
        IrrationalNumber(3, 16),
        "2root(3,2)"
    );

    test(
        "cbrt(54) = 3root(3,2)",
        IrrationalNumber(3, 54),
        "3root(3,2)"
    );


    // =====================================================
    // 4. 5���� �ܼ�ȭ
    // =====================================================

    test(
        "root(5,32) = 2",
        IrrationalNumber(5, 32),
        "2"
    );

    test(
        "root(5,64) = 2root(5,2)",
        IrrationalNumber(5, 64),
        "2root(5,2)"
    );


    // =====================================================
    // 5. ���� ��ȣ�� ����
    // =====================================================

    IrrationalNumber cbrt2(3, 2);

    test(
        "cbrt(2) + cbrt(2)",
        cbrt2 + cbrt2,
        "2root(3,2)"
    );

    test(
        "sqrt(2) + sqrt(2)",
        IrrationalNumber(2, 2)
        + IrrationalNumber(2, 2),
        "2sqrt(2)"
    );


    // =====================================================
    // 6. ���� ��ȣ�� ����
    // =====================================================

    test(
        "cbrt(2) - cbrt(2)",
        cbrt2 - cbrt2,
        "0"
    );

    test(
        "sqrt(2) - sqrt(2)",
        IrrationalNumber(2, 2)
        - IrrationalNumber(2, 2),
        "0"
    );


    // =====================================================
    // 7. �Ϲ� ��ȣ ����
    // =====================================================

    test(
        "cbrt(2) * cbrt(2)",
        IrrationalNumber(3, 2)
        * IrrationalNumber(3, 2),
        "root(3,4)"
    );

    test(
        "cbrt(2) * cbrt(4)",
        IrrationalNumber(3, 2)
        * IrrationalNumber(3, 4),
        "2"
    );

    test(
        "sqrt(2) * sqrt(3)",
        IrrationalNumber(2, 2)
        * IrrationalNumber(2, 3),
        "sqrt(6)"
    );

    test(
        "sqrt(2) * sqrt(8)",
        IrrationalNumber(2, 2)
        * IrrationalNumber(2, 8),
        "4"
    );


    // =====================================================
    // 8. ���� �ٸ� ������ ��ȣ ����
    // =====================================================

    test(
        "sqrt(2) * cbrt(2)",
        IrrationalNumber(2, 2)
        * IrrationalNumber(3, 2),
        "root(6,32)"
    );


    // =====================================================
    // 9. ����� �ִ� �Ϲ� ��ȣ
    // =====================================================

    test(
        "2cbrt(2)",
        IrrationalNumber(
            Fraction<long long>(2),
            3,
            2
        ),
        "2root(3,2)"
    );

    test(
        "1/2 sqrt(2)",
        IrrationalNumber(
            Fraction<long long>(1, 2),
            2,
            2
        ),
        "1/2sqrt(2)"
    );


    // =====================================================
    // 10. �ŵ�����
    // =====================================================

    test(
        "sqrt(2)^2",
        IrrationalNumber(2, 2) ^ 2,
        "2"
    );

    test(
        "sqrt(2)^3",
        IrrationalNumber(2, 2) ^ 3,
        "2sqrt(2)"
    );

    test(
        "cbrt(2)^2",
        IrrationalNumber(3, 2) ^ 2,
        "root(3,4)"
    );

    test(
        "cbrt(2)^3",
        IrrationalNumber(3, 2) ^ 3,
        "2"
    );

    test(
        "root(5,2)^5",
        IrrationalNumber(5, 2) ^ 5,
        "2"
    );


    // =====================================================
    // 11. ���� �ŵ�����
    // =====================================================

    test(
        "sqrt(2)^-2",
        IrrationalNumber(2, 2) ^ -2,
        "1/2"
    );


    // =====================================================
    // 12. �������� �Ϲ� ��ȣ ȥ��
    // =====================================================

    test(
        "2 + cbrt(2)",
        IrrationalNumber(2)
        + IrrationalNumber(3, 2),
        "2 + root(3,2)"
    );

    test(
        "3 - cbrt(2)",
        IrrationalNumber(3)
        - IrrationalNumber(3, 2),
        "3 - root(3,2)"
    );

    test(
        "2 * cbrt(2)",
        IrrationalNumber(2)
        * IrrationalNumber(3, 2),
        "2root(3,2)"
    );


    // =====================================================
    // 13. �߸��� ��ȣ
    // =====================================================

    testException(
        "root(1,2)",
        []()
        {
            IrrationalNumber invalid(1, 2);
        }
    );

    testException(
        "root(0,2)",
        []()
        {
            IrrationalNumber invalid(0, 2);
        }
    );

    testException(
        "root(3,0)",
        []()
        {
            IrrationalNumber invalid(3, 0);
        }
    );


    // =====================================================
    // �׽�Ʈ ���
    // =====================================================

    cout << endl;
    cout << "===== �׽�Ʈ ��� ====="
         << endl;

    cout << "���: "
         << passCount
         << endl;

    cout << "����: "
         << failCount
         << endl;

    cout << "��ü: "
         << passCount + failCount
         << endl;

    cout << endl;

    if (failCount == 0)
    {
        cout << "��� �׽�Ʈ ���!"
             << endl;

        return 0;
    }

    cout << "�Ϻ� �׽�Ʈ ����"
         << endl;

    return 1;
}
