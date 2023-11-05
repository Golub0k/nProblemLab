import random
import argparse


def handle_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('bodies_count', action='store',
                        type=int, help='Number of bodies')
    parser.add_argument('steps', action='store',
                        type=int, help='Number of steps')
    parser.add_argument('filename', action='store', type=str,
                        help='Destination of generation')

    parser.add_argument(
        '-minMass', type=float, help="Minimum mass for body (default=10000.0)", default=10000.0)
    parser.add_argument(
        '-maxMass', type=float, help="Maximum mass for body (default=50000.0)", default=50000.0)
    parser.add_argument(
        '-minXPos', type=float, help="Minimum x position for body (default=-5.0)", default=-5.0)
    parser.add_argument(
        '-maxXPos', type=float, help="Maximum x position for body (default=5.0)", default=5.0)
    parser.add_argument(
        '-minYPos', type=float, help="Minimum y position for body (default=-5.0)", default=-5.0)
    parser.add_argument(
        '-maxYPos', type=float, help="Maximum y position for body (default=5.0)", default=5.0)
    parser.add_argument(
        '-minXSpeed', type=float, help="Minimum speed projection on x for body (default=-5.0)", default=-5.0)
    parser.add_argument(
        '-maxXSpeed', type=float, help="Maximum speed projection on x for body (default=5.0)", default=5.0)
    parser.add_argument(
        '-minYSpeed', type=float, help="Minimum speed projection on y for body (default=-5.0)", default=-5.0)
    parser.add_argument(
        '-maxYSpeed', type=float, help="Maximum speed projection on y for body (default=5.0)", default=5.0)

    return parser.parse_args()


def generate(args):
    open(args.filename, 'w').close()
    with open(args.filename, 'a') as f:
        f.write(f'6.6743e-11 {args.bodies_count} {args.steps}\n')
        for _ in range(0, args.bodies_count):
            mass = random.uniform(args.minMass, args.maxMass)
            x = random.uniform(args.minXPos, args.maxXPos)
            y = random.uniform(args.minYPos, args.maxYPos)
            vx = random.uniform(args.minXSpeed, args.maxXSpeed)
            vy = random.uniform(args.minYSpeed, args.maxYSpeed)

            f.write(f'{mass}\n{x} {y}\n{vx} {vy}\n')


if __name__ == '__main__':
    args = handle_args()
    generate(args)