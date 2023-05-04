import { readdir } from "node:fs/promises";

const BASE_MAIL = "C:/Users/Rafael/Desktop/Code/low-level/smtp/messages"

/** @type {import('./$types').PageLoad} */
export async function load() {

    const accounts: {
        address: string, emails: {
            id: string;
            date: Date;
        }[]
    }[] = [];

    const accountsEmails = await readdir(BASE_MAIL);
    for (const addr of accountsEmails) {
        const emails = await readdir(BASE_MAIL + "/" + addr);
        accounts.push({
            address: addr,
            emails: emails.map(e => {
                let date = new Date(e.split(".")[0]?.replaceAll("\uf03a", ":"));
                if (isNaN(date as unknown as number)) date = new Date();
                return {
                    id: e,
                    date
                }
            }),
        })
    }

    return {
        accounts
    };
}