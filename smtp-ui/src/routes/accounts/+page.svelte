<script lang="ts">
	import { readEmails } from '../../lib/stores/emails';
	import { flip } from 'svelte/animate';
	import Icon from '@iconify/svelte';
	import type { PageData } from './$types';
	import { toRelativeTime } from '$lib/utils/time';

	export let data: PageData;
	const targetDate = new Date();

	let filter = '';
</script>

<div class="flex items-stretch flex-col font-[Poppins]">
	<input
		type="text"
		placeholder="Search an email"
		bind:value={filter}
		class="bg-slate-800 border-slate-700 focus:ring-transparent focus:outline-none placeholder:text-slate-500 shadow-slate-800 box-border p-2 m-2 text-white border rounded-sm shadow-sm"
	/>
	<ul>
		{#each data.accounts as account (account.address)}
			{@const unread = account.emails.filter((e) => !$readEmails.includes(e.date.getTime())).length}
			{@const nearestDate = account.emails.reduce((a, b) =>
				Math.abs(+b.date - +targetDate) < Math.abs(+a.date - +targetDate) ? b : a
			)}

			<li animate:flip={{ duration: 300 }}>
				{#if account.address.includes(filter)}
					<a href="/accounts/{account.address}" class="item flex gap-2">
						<button
							class="aspect-square hover:bg-slate-700 hover:border-slate-600 flex items-center justify-center w-6 h-6 border border-transparent rounded"
						>
							<Icon icon="material-symbols:block" />
						</button>
						<div class="item grow flex justify-between">
							{account.address}
							<div class="">
								{toRelativeTime(nearestDate.date)}
								{#if unread == account.emails.length}
									<span class="font-bold">({account.emails.length})</span>
								{:else}
									{account.emails.length}
								{/if}
							</div>
						</div>
					</a>
				{/if}
			</li>
		{/each}
	</ul>
</div>

<style lang="postcss">
	li > .item {
		@apply w-full p-2 border border-transparent border-b-slate-800 text-slate-400 cursor-pointer ease-out  transition-all;
	}
	li > .item:hover {
		background: theme(backgroundColor.slate.800);
		border: 1px solid theme(borderColor.slate.700);
	}
</style>
