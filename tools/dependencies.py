#!/usr/bin/env python3
"""Rank direct branch dependencies; keep the full research graph Git-ignored.

This is a static instruction scan, not a recovered call graph or type system.
Only sized executable function symbols are scanned. No matching credit is earned.
"""
import bisect
from collections import Counter, defaultdict
import hashlib
import json
import struct

from audit import load_target
from code_map import cluster_functions
from setup import CONFIG, ROOT


def branch(word, address):
    """Decode PowerPC I/B-form branches and register-indirect branch-and-link."""
    opcode = word >> 26
    if opcode in (18, 16):
        bits = 26 if opcode == 18 else 16
        displacement = word & ((1 << bits) - 4)
        if displacement & (1 << (bits - 1)):
            displacement -= 1 << bits
        target = (displacement + (0 if word & 2 else address)) & 0xffffffff
        conditional = opcode == 16 and ((word >> 21) & 0x14) != 0x14
        return dict(target=target, linked=bool(word & 1), conditional=conditional)
    if opcode == 19 and word & 1 and ((word >> 1) & 1023) in (16, 528):
        return dict(target=None, linked=True, conditional=((word >> 21) & 0x14) != 0x14)
    return None


def dependency_graph(elf, accepted=()):
    sections = {s['index']: s for s in elf.sections if s['flags'] & 6 == 6}
    functions = [s for s in elf.symbols() if s['type'] == 2 and s['size'] > 0
                 and s['section'] in sections]
    for f in functions:
        section = sections[f['section']]
        if (f['address'] % 4 or f['size'] % 4 or not
                section['address'] <= f['address'] < f['address'] + f['size']
                <= section['address'] + section['size']):
            raise ValueError('Function is outside its section or not instruction-aligned')
    clusters = sorted(cluster_functions(functions), key=lambda c: c['start'])
    if any(a['end'] > b['start'] for a, b in zip(clusters, clusters[1:])):
        raise ValueError('Executable sections overlap')
    starts = [c['start'] for c in clusters]
    accepted = set(accepted)
    nodes = [dict(address=hex(c['start']), size=c['end'] - c['start'],
                  names=sorted({s['name'] for s in c['symbols']}),
                  source_accepted=all((s['name'], s['address'], s['size']) in accepted
                                      for s in c['symbols']),
                  shared_range=len(c['symbols']) > 1)
             for c in clusters]
    edges, unresolved, indirect = [], [], Counter()
    for source, cluster in enumerate(clusters):
        section = sections[cluster['section']]
        data = elf.contents(section)
        for address in range(cluster['start'], cluster['end'], 4):
            word = struct.unpack_from('>I', data, address - section['address'])[0]
            decoded = branch(word, address)
            if decoded is None:
                continue
            if decoded['target'] is None:
                indirect[source] += 1
                continue
            target = decoded['target']
            index = bisect.bisect_right(starts, target) - 1
            target_cluster = clusters[index] if index >= 0 and target < clusters[index]['end'] else None
            # Ordinary loops/conditionals are not dependencies. Preserve recursive
            # calls only when the linked target is an actual symbol entry point.
            entries = ([s for s in target_cluster['symbols'] if s['address'] == target]
                       if target_cluster is not None else [])
            if index == source and (not decoded['linked'] or not entries):
                continue
            record = dict(source=source, site=hex(address), target_address=hex(target),
                          kind='call' if decoded['linked'] else 'branch',
                          conditional=decoded['conditional'])
            if entries:
                record.update(target=index, target_names=sorted({s['name'] for s in entries}))
                edges.append(record)
            else:
                # Do not guess an interior target's callee or assign unknown code.
                record['reason'] = 'interior-function-target' if target_cluster else 'no-function-entry'
                unresolved.append(record)
    for index, count in indirect.items():
        nodes[index]['indirect_call_sites'] = count
    incoming = defaultdict(set)
    sites = Counter()
    for edge in edges:
        if edge['source'] != edge['target']:
            incoming[edge['target']].add(edge['source'])
            sites[edge['target']] += 1
    ranked = []
    for target, callers in incoming.items():
        unfinished = {i for i in callers if not nodes[i]['source_accepted']}
        ranked.append(dict(node=target, callers=len(callers), unfinished_callers=len(unfinished),
                           sites=sites[target],
                           unfinished_caller_bytes=sum(nodes[i]['size'] for i in unfinished)))
    ranked.sort(key=lambda x: (-x['unfinished_callers'], -x['unfinished_caller_bytes'], x['node']))
    return dict(schema_version=1, nodes=nodes, edges=edges, unresolved=unresolved,
                ranked=ranked, summary=dict(function_ranges=len(nodes),
                direct_dependency_sites=len(edges), unresolved_direct_sites=len(unresolved),
                indirect_call_sites=sum(indirect.values())), limitations=[
                    'Direct instruction references only; conditional paths may be unreachable.',
                    'Unlinked inter-function branches are candidates, not proven tail calls.',
                    'Register-indirect calls are counted but their targets are not inferred.',
                    'Overlapping symbols share a node; no unique caller identity is inferred.',
                    'No vtable, data-reference, class-layout or dynamic-execution recovery.',
                    'Caller sizes are unique within each row, overlap across rows, and are not progress.'])


def main():
    _, elf = load_target()
    config = CONFIG
    project = json.loads((config / 'project.json').read_text())
    accepted = [(f['name'], int(f['address'], 16), f['size'])
                for path in project['units']
                for f in json.loads((config / path).read_text())['functions']]
    result = dependency_graph(elf, accepted)
    result['target_sha256'] = hashlib.sha256(elf.data).hexdigest()
    output = ROOT / 'build/audit/dependencies.json'
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(result['summary'], indent=2))
    print('Highest fan-in unfinished helpers (unique unfinished caller ranges):')
    for row in [r for r in result['ranked'] if not result['nodes'][r['node']]['source_accepted']][:20]:
        node = result['nodes'][row['node']]
        print(f"{row['unfinished_callers']:4d} callers | {node['size']:5d} bytes | {node['names'][0]}")
    print('Research graph: build/audit/dependencies.json (ignored; no code credit)')


if __name__ == '__main__':
    main()
