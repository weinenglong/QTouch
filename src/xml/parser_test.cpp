/**
 * \file courseparser_test.cpp
 *
 * \date 06.07.2015
 * \author Moritz Nisblé moritz.nisble@gmx.de
 */

#include <QtTest/QtTest>
#include <QLinkedList>
#include <QDir>

#include "parser.hpp"

namespace qtouch
{
namespace xml
{

const int TestcourseLessonCount = 2;

class XmlParserTest: public QObject
{
	Q_OBJECT

private slots:
	// will be called before the first test function is executed
	void initTestCase();
	//  will be called after the last test function was executed.
	//	void cleanupTestCase();
	// will be called before each test function is executed.
	//	void init();
	//  will be called after every test function.
	//	void cleanup();

	void invalidSchemaFilePath();
	void invalidXmlFilePath();

	void invalidSchemaFile();

	void validation();
	void parseTestCourse();
	void parseCourses();

private:
	void verifyTestCourse(const CoursePtr& c);

	ValidatorPtr validator;
};

void XmlParserTest::verifyTestCourse(const CoursePtr& c)
{
	QCOMPARE(c->getId().toString(), QStringLiteral("{4e007d5e-613a-4a26-bff3-658d44d9cf10}"));
	QCOMPARE(c->getTitle(), QStringLiteral("TestCourse"));
	QCOMPARE(c->getDescription(),
	         QStringLiteral("This file is only for testing purposes and will be filtered by application."));
	QCOMPARE(c->isBuiltin(), true);

	// TODO: Add keyboard layout

	QVERIFY2(c->lessonCount() == TestcourseLessonCount, "Wrong lesson count");

	Course::const_iterator it = c->begin();

	QCOMPARE((*it)->getId().toString(), QStringLiteral("{d6e5a9a9-3c31-4175-8d58-245695c60b08}"));
	QCOMPARE((*it)->getTitle(), QStringLiteral("TestLesson1"));
	QCOMPARE((*it)->getNewChars(), QStringLiteral("fj"));
	QCOMPARE((*it)->getText(), QStringLiteral("fff jjj"));
	QCOMPARE((*it)->isBuiltin(), true);

	++it;

	QCOMPARE((*it)->getId().toString(), QStringLiteral("{e169c06b-71be-46df-97cf-8b3e68c5f75d}"));
	QCOMPARE((*it)->getTitle(), QStringLiteral("TestLesson2"));
	QCOMPARE((*it)->getNewChars(), QStringLiteral("dk"));
	QCOMPARE((*it)->getText(), QStringLiteral("ddd kkk"));
	QCOMPARE((*it)->isBuiltin(), true);
}

void XmlParserTest::initTestCase()
{
	try
	{
		// Create a validator
		validator = createValidator(QStringLiteral(":/courses/course.xsd"));
	}
	catch (Exception& e)
	{
		QFAIL(qUtf8Printable(e.message()));
	}
}

void XmlParserTest::invalidSchemaFilePath()
{
	QVERIFY_EXCEPTION_THROWN(createValidator(QStringLiteral(":/testing/courses/FOOBAR.xsd")), FileException);
}

void XmlParserTest::invalidXmlFilePath()
{
	QVERIFY_EXCEPTION_THROWN(validate(QStringLiteral(":/testing/courses/FOOBAR.xml"), validator),
	                         FileException);
}

void XmlParserTest::invalidSchemaFile()
{
	QVERIFY_EXCEPTION_THROWN(createValidator(QStringLiteral(":/testing/courses/testcourse.xml")), XmlException);
}

void XmlParserTest::validation()
{
	QCOMPARE(validate(QStringLiteral(":/testing/courses/testcourse.xml"), validator), true);
}

void XmlParserTest::parseTestCourse()
{
	ParseResult result;
	QString message;

	CoursePtr course;

	try
	{
		course = parseCourse(QStringLiteral(":/testing/courses/testcourse.xml"), validator, &result,
		                     &message);
	}
	catch (Exception& e)
	{
		QFAIL(qUtf8Printable(e.message()));
	}

	verifyTestCourse(course);
}

void XmlParserTest::parseCourses()
{
	QDir coursepath(QStringLiteral(":/courses"), "*.xml", QDir::Name | QDir::IgnoreCase, QDir::Files);

	QStringList coursefiles;
	foreach(QString const & s, coursepath.entryList())
	{
		coursefiles.append(coursepath.filePath(s));
	}

	qDebug() << coursefiles;

	CourseList courseList;

	QStringListIterator it(coursefiles);
	while (it.hasNext())
	{
		CoursePtr course;

		ParseResult result;
		QString message;

		try
		{
			course = parseCourse(it.next(), validator, &result, &message);

			if (result != Ok)
			{
				qWarning() << "Result:" << result << " " << message;
			}

			courseList.append(course);
		}
		catch (Exception& e)
		{
			QFAIL(qUtf8Printable(e.message()));
		}
	}

	QCOMPARE(coursepath.entryList().size(), courseList.length());
}

} /* namespace xml */
} /* namespace qtouch */

QTEST_GUILESS_MAIN(qtouch::xml::XmlParserTest)
#include "parser_test.moc"