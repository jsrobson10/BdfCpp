
#include <iostream>

#include <bdf/Bdf.hpp>

int testNumber = 0;

void test(bool result)
{
	testNumber++;

	if(result)
	{
		std::cout << "Test " << testNumber << " passed\n";
	}

	else
	{
		std::cout << "Test " << testNumber << " failed\n";
		throw std::runtime_error("breakpoint");
	}
}

int main()
{
	Bdf::BdfReader reader;
	Bdf::BdfObject* bdf = reader.getObject();
	Bdf::BdfList* list = bdf->getList();

	test(list->size() == 0);

	list->add(bdf->newObject()->setInteger(69));
	list->add(bdf->newObject()->setInteger(420));

	test(list->size() == 2);
	test(list->get(1)->getInteger() == 420);
	test(list->get(0)->getInteger() == 69);
	test(list->getStart() != NULL);
	test(list->getEnd() != NULL);
	test(list->getStart()->next != NULL);
	test(list->getStart()->object != NULL);
	test(list->getStart()->next->object != NULL);
	test(list->getStart()->object->getInteger() == 69);
	test(list->getStart()->next->object->getInteger() == 420);
	test(list->getEnd()->object->getInteger() == 420);
	test(list->getEnd()->last->object->getInteger() == 69);

	list->add(bdf->newObject()->setInteger(12345));

	test(list->size() == 3);
	test(list->getStart()->object->getInteger() == 69);
	test(list->getStart()->next->object->getInteger() == 420);
	test(list->get(2)->getInteger() == 12345);
	test(list->getEnd()->object->getInteger() == 12345);

	list->remove(list->getStart());

	test(list->size() == 2);
	test(list->getStart()->object->getInteger() == 420);
	test(list->getStart()->next->object->getInteger() == 12345);

	list->set(1, bdf->newObject()->setInteger(55));
	
	test(list->size() == 2);
	test(list->get(0)->getInteger() == 420);
	test(list->get(1)->getInteger() == 55);

	list->remove(1);

	test(list->size() == 1);

	list->remove(list->getStart()->object);

	test(list->size() == 0);
	test(list->getStart() == NULL);
	test(list->getEnd() == NULL);

	list->add(bdf->newObject()->setInteger(12345));
	list->insertNext(list->getEnd(), bdf->newObject()->setInteger(42));
	list->insertNext(list->getStart(), bdf->newObject()->setInteger(69));
	list->insertLast(list->getStart(), bdf->newObject()->setInteger(54321));
	list->insertLast(list->getEnd(), bdf->newObject()->setInteger(420));

	// 54321, 12345, 69, 420, 42

	test(list->get(0)->getInteger() == 54321);
	test(list->get(1)->getInteger() == 12345);
	test(list->get(2)->getInteger() == 69);
	test(list->get(3)->getInteger() == 420);
	test(list->get(4)->getInteger() == 42);
	test(list->getEnd()->object->getInteger() == 42);
	test(list->getEnd()->last->object->getInteger() == 420);
	test(list->getEnd()->last->last->object->getInteger() == 69);
	test(list->getEnd()->last->last->last->object->getInteger() == 12345);
	test(list->getEnd()->last->last->last->last->object->getInteger() == 54321);

	Bdf::BdfNamedList* nl = bdf->newNamedList();
	list->add(bdf->newObject()->setNamedList(nl));

	test(list->size() == 6);

	nl->set("Hello", bdf->newObject()->setString("World!"));

	test(nl->get("Hello")->getString() == "World!");

	nl->set("Hello", bdf->newObject()->setInteger(42));

	test(nl->get("Hello")->getInteger() == 42);

	test(!nl->exists("test"));

	nl->set("test", bdf->newObject()->setInteger(69));

	test(nl->exists("test"));
	test(nl->get("test")->getInteger() == 69);
	test(nl->exists("Hello"));

	nl->remove("test");

	test(!nl->exists("test"));

	return 0;
}
