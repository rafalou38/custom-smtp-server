import { simpleParser } from "mailparser";


simpleParser(`O:<bar@example.com>

Content-Type: multipart/alternative; boundary="--_NmP-7bd80925b63b9578-Part_1"
From: =?UTF-8?Q?Fred_Foo_=F0=9F=91=BB?= <foo@example.com>
To: bar@example.com
Subject: =?UTF-8?Q?Hello_=E2=9C=94?=
Message-ID: <136f1662-99ec-eedb-7e4d-911c6e775bc4@example.com>
Date: Tue, 25 Apr 2023 19:36:52 +0000
MIME-Version: 1.0

----_NmP-7bd80925b63b9578-Part_1
Content-Type: text/plain; charset=utf-8
Content-Transfer-Encoding: 7bit

Hello world?
----_NmP-7bd80925b63b9578-Part_1
Content-Type: text/html; charset=utf-8
Content-Transfer-Encoding: 7bit

<b>Hello world?</b>
----_NmP-7bd80925b63b9578-Part_1--`).then(console.log)