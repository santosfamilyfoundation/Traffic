#ifndef READNUMBER_H_
#define READNUMBER_H_

unsigned readUnsignedInt(const char *msg)
{
	unsigned nbOfPoints;

	bool error;

	do
	{
		error = false;
		std::cout << msg;
		std::cin >> nbOfPoints;
		if (std::cin.fail())
		{
			std::cout << "Please enter a valid unsigned integer." << std::endl;
			error = true;
			std::cin.clear();
			std::cin.ignore(80, '\n');
		}
	} while (error);

	return nbOfPoints;
}

#endif /* READNUMBER_H_ */
