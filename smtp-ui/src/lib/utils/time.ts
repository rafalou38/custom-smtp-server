export function toRelativeTime(date: Date) {
    const now = new Date();
    const diff = now.getTime() - date.getTime();
    const minute = 60 * 1000;
    const hour = 60 * minute;
    const day = 24 * hour;

    if (diff < minute) {
        return Math.floor(diff / 1000) + 's';
    } else if (diff < hour) {
        return Math.floor(diff / minute) + 'm';
    } else if (diff < day) {
        return Math.floor(diff / hour) + 'h';
    } else {
        return Math.floor(diff / day) + 'd';
    }
}
