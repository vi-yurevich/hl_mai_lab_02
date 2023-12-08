use archdb; 

CREATE TABLE IF NOT EXISTS `User` (
    `id` INT NOT NULL AUTO_INCREMENT,
    `first_name` VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
    `last_name` VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
    `email` VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NULL,
    `phone_number` VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NULL,
    `login` VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NULL,
    `password` VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NULL,
    PRIMARY KEY (`id`),
    INDEX `fn` (`first_name`),
    INDEX `ln` (`last_name`));

CREATE TABLE IF NOT EXISTS `Conference` (
    `id` INT NOT NULL AUTO_INCREMENT, 
    `location` VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
    `title` VARCHAR(2048) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
    `description` VARCHAR(4096) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
    `format` VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
    PRIMARY KEY (`id`));

CREATE TABLE IF NOT EXISTS `Lecture` (
    `id` INT NOT NULL AUTO_INCREMENT,
    `title` VARCHAR(2048) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
    `description` VARCHAR(4096) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
    `record_permission` BOOLEAN NOT NULL,
    `author_id` INT NOT NULL,
    `conference_id` INT NOT NULL,
    PRIMARY KEY (`id`),
    FOREIGN KEY (`author_id`) REFERENCES `User` (`id`),
    FOREIGN KEY (`conference_id`) REFERENCES `Conference` (`id`)
);

INSERT INTO `User` (first_name, last_name, email, phone_number, `login`, `password`) values
('Ivan', 'Ivanov', 'ivanov@example.com', '+79001112233', 'ivanivanov', '123456234'),
('Maria', 'Petrova', 'petrova@example.net', '+79112223344', 'mpetrova', '1234234'),
('Sergey', 'Sidorov', 'sidorov@example.org', '+79223334455', 'ssidorov', '1232342'),
('Anna', 'Kuznetsova', 'kuznetsova@example.com', '+79334445566', 'akuznetsova', '1234234'),
('Evgeny', 'Alekseev', 'alekseev@example.info', '+79445556677', 'ealekseev', '2234234'),
('Tatyana', 'Vasilieva', 'vasilieva@example.net', '+79556667788', 'tvasilieva', '324234'),
('Nikolay', 'Nikolaev', 'nikolaev@example.org', '+79667778899', 'nikolaev1', '1234'), 
('Olga', 'Smirnova', 'smirnova@example.com', '+79778889900', 'smirnova1', '567834'),
('Andrey', 'Andreev', 'andreev@example.info', '+79889990011', 'andreev1', '904353'),
('Elena', 'Kozlova', 'kozlova@example.net', '+79900001122', 'kozlova1', '0345345435'),
('Victor', 'Fedorov', 'fedorov@example.org', '+79011112233', 'fedorov1', '1145435'),
('Roman', 'Romanov', 'romanov@example.com', '+79122223344', 'romanov1', '2212213'),
('Anastasia', 'Novikova', 'novikova@example.net', '+79233334455', 'anastas', '123123'),
('Boris', 'Borisov', 'borisov@example.org', '+79344445566', 'borboris', '203203'),
('Galina', 'Grigorieva', 'grigoreva@example.com', '+79455556677', 'ggrigor', '203012301'),
('Dmitry', 'Dmitriev', 'dmitriev@example.info', '+79566667788', 'dmitrydm', '203120301'),
('Evgenia', 'Ezhova', 'ezhova@example.net', '+79677778899', 'ezevia', '3090193021');

INSERT INTO Conference (`location`, `title`, `description`, `format`) VALUES
(
    'Moscow City Conference Center',
    'Scala Masters',
    'This conference is dedicated to one of the most popular programming languages - Scala. We will discuss the latest trends and innovations in the field of Scala development, share our experience in solving complex problems, and present new libraries and frameworks.',
    'face-to-face plus broadcast'
);

SET @last_conference_id = LAST_INSERT_ID();


INSERT INTO Lecture (`title`, `description`, record_permission, author_id, conference_id) values
('Mastering Scala: Building Scalable Systems',
'This talk is about using Scala to create scalable systems. We will look at different design and development approaches and discuss best practices and tools for working with Scala.',
TRUE,
1,
@last_conference_id),

('Functional Programming in Scala',
'In this talk we will get acquainted with functional programming and its application in Scala. We will explore the basic concepts and principles of functional programming and look at examples of how to use them in Scala.',
TRUE,
2,
@last_conference_id),

('Testing and Debugging in Scala',
'The report is dedicated to testing and debugging code in the Scala language. We will look at basic testing approaches, discuss different types of tests, and introduce tools for debugging and analyzing code.',
TRUE,
3,
@last_conference_id),

('Advanced Scala Patterns',
'In this talk we will delve into the study of design patterns in Scala. Lets look at various patterns and their applications, and also give examples of their use in real projects.',
TRUE,
4,
@last_conference_id),

('Microservices with Scala and Akka',
'In the report we will look at the use of Scala to build microservice architectures. We will get acquainted with the Akka framework, which allows you to create scalable and reliable services, and also present examples of using Akka in real projects.',
TRUE,
5,
@last_conference_id),

('Sparkling with Scala: Big Data Analytics',
'In this talk, we will explore the capabilities of Scala for big data analysis. We will introduce students to the Spark library and show how to use Scala for data processing and analysis.',
TRUE,
6,
@last_conference_id),

('Securing Scala Applications',
'This report will discuss the security of applications in the Scala language. We will talk about how to protect against attacks, what security measures should be taken, and what tools to use for this purpose.',
TRUE,
7,
@last_conference_id),

('Scala for the Frontend',
'This talk will talk about using Scala for developing web applications. We will explore the libraries and tools that let you use Scala to create interactive and dynamic interfaces.',
TRUE,
8,
@last_conference_id),

('Designing with Scala: Best Practices and Patterns',
'The report is dedicated to design practices and patterns in the Scala language. We will talk about how to properly develop applications using Scala, what mistakes can be made and how to avoid them.',
TRUE,
9,
@last_conference_id),

('Building a Scalable Team: Tips for Scala Developers',
'Final talk in which we will talk about what it takes to build a successful Scala development team.',
TRUE,
10,
@last_conference_id);